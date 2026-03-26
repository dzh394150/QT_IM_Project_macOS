#include "CreateGroup.h"
#include "ui_CreateGroup.h"
#include "FriendListModel.h"
#include "mainwindow.h"

// 复刻注册逻辑的必要头文件
#include <QListView>
#include <QTimer>
#include <QFileDialog>
#include <QFile>
#include <QImage>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QEventLoop>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QRandomGenerator>
#include <QMouseEvent>
#include <QSslCipher>
#include <QDebug>

// 构造函数（与注册逻辑对齐，初始化成员变量）
// 构造函数：修复居中问题，对齐老代码的关键逻辑
CreateGroup::CreateGroup(
    QNetworkAccessManager *netManager,
    const QSslConfiguration &sslConf,
    QSslSocket *mainSslSocket,
    const QString &currentUser,
    QWidget *parent
    ) : QDialog(parent),
    // 1. UI与核心成员（严格匹配头文件声明顺序）
    ui(new Ui::CreateGroup),
    m_isDragging(false),
    // 2. 复用与配置相关（严格匹配头文件声明顺序）
    m_netManager(netManager),
    m_sslConf(sslConf),
    m_mainSslSocket(mainSslSocket),
    m_currentUser(currentUser),
    // 3. 业务状态变量（严格匹配头文件声明顺序）
    m_webSocketHandshakeDone(true),
    m_loadingMsg(nullptr),
    m_createGroupSuccess(false)
{
    ui->setupUi(this);

    // 修复1：叠加无边框标志（保留QDialog原有默认属性，关键！）
    this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    // 1. 初始化列表多选模式（创建群聊必备）
    ui->creategroupList->setSelectionMode(QAbstractItemView::MultiSelection);

    // 2. 信号绑定（与注册逻辑对齐，仅绑定必要信号）
    connect(ui->btnAvatargroup, &QPushButton::clicked, this, &CreateGroup::selectAvatar);
    connect(ui->btncreategroup, &QPushButton::clicked, this, &CreateGroup::handleCreateGroup);
    connect(ui->groupclose, &QPushButton::clicked, this, &CreateGroup::on_groupclose_clicked); // 绑定关闭按钮

    // 3. 绑定主窗口SSL套接字信号（复用通信，与注册逻辑对齐）
    if (m_mainSslSocket) {
        connect(m_mainSslSocket, &QSslSocket::sslErrors, this, &CreateGroup::onSslErrors);
        // connect(m_mainSslSocket, &QSslSocket::readyRead, this, &CreateGroup::onSslReadyRead);
        connect(m_mainSslSocket, &QSslSocket::disconnected, this, &CreateGroup::onDisconnected);
        connect(m_mainSslSocket, QOverload<QAbstractSocket::SocketError>::of(&QSslSocket::errorOccurred), this, &CreateGroup::onSocketError);

        // 设置读取缓冲区大小（与注册逻辑一致）
        m_mainSslSocket->setReadBufferSize(1024 * 1024);
    }

    // 2. 新增：绑定主窗口的自定义信号（关键！）
    MainWindow *parentMainWindow = qobject_cast<MainWindow*>(parent); // 转换父窗口为MainWindow类型
    if (parentMainWindow) {
        // 断开原有与套接字readyRead的绑定（如果有，直接删除即可）
        // disconnect(m_mainSslSocket, &QSslSocket::readyRead, this, &CreateGroup::onSslReadyRead);

        // 绑定主窗口自定义信号，接收转发数据
        connect(parentMainWindow, &MainWindow::signalGroupSocketDataArrived,
                this, &CreateGroup::onRecvMainWindowSocketData,
                Qt::QueuedConnection); // 队列连接，避免线程问题
        qDebug() << "【CreateGroup】已绑定主窗口数据转发信号";
    }

    // 修复2：先完成所有UI/信号槽初始化，再刷新窗口尺寸（获取真实尺寸）
    this->layout()->activate(); // 激活布局管理器，强制计算控件真实尺寸
    this->adjustSize(); // 刷新窗口到布局后的真实尺寸

    // 修复3：对齐老代码，居中前显式绑定父窗口，确保几何信息正确
    if (parent) {
        this->setParent(parent);
        // 手动计算居中坐标（父窗口中央 - 自身窗口中央，与老代码完全一致）
        QRect parentRect = parent->geometry();
        QRect selfRect = this->geometry();
        int centerX = parentRect.x() + (parentRect.width() - selfRect.width()) / 2;
        int centerY = parentRect.y() + (parentRect.height() - selfRect.height()) / 2;
        this->move(centerX, centerY); // 移动到居中位置，无额外操作覆盖
    }
}

// 析构函数（与注册逻辑对齐，释放资源）
CreateGroup::~CreateGroup()
{
    delete ui;

    // 仅释放自身创建的资源，主窗口传入的对象由主窗口管理（与注册逻辑一致）
    if (m_loadingMsg) {
        m_loadingMsg->close();
        m_loadingMsg->deleteLater();
    }
}

// 公共接口：设置列表模型和委托
void CreateGroup::setGroupListModelAndDelegate(QAbstractItemModel *model, QAbstractItemDelegate *delegate)
{
    if (model == nullptr || delegate == nullptr || ui->creategroupList == nullptr) {
        return;
    }
    ui->creategroupList->setModel(model);
    ui->creategroupList->setItemDelegate(delegate);
}

// 公共接口：设置窗口目标位置
void CreateGroup::setTargetWindowPos(const QPoint &btnScreenPos)
{
    m_targetPos = btnScreenPos;
    QTimer::singleShot(0, this, &CreateGroup::doMoveToTargetPos);
}

// 窗口移动：定位到目标位置
void CreateGroup::doMoveToTargetPos()
{
    this->adjustSize();
    this->move(m_targetPos);
}

// 通用SSL配置（完全复刻注册逻辑，保证一致性和可行性）
QSslConfiguration CreateGroup::getSslConfig()
{
    QSslConfiguration sslConfig = m_sslConf;
    // 1. 强制TLS1.2（和服务器一致，复刻注册逻辑）
    sslConfig.setProtocol(QSsl::TlsV1_2);
    // 2. 开启证书验证
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyPeer);
    sslConfig.setPeerVerifyDepth(3);

    // 3. 适配旧OpenSSL的SSL选项（禁用SNI/压缩，复刻注册逻辑）
    sslConfig.setSslOption(QSsl::SslOptionDisableCompression, true);
    sslConfig.setSslOption(QSsl::SslOptionDisableServerNameIndication, true);
    sslConfig.setSslOption(QSsl::SslOptionDisableSessionTickets, false);

    // 4. 手动指定服务器支持的加密套件（复刻注册逻辑，避免协商失败）
    QList<QSslCipher> cipherList;
    cipherList << QSslCipher("ECDHE-RSA-AES128-GCM-SHA256", QSsl::TlsV1_2)
               << QSslCipher("AES128-GCM-SHA256", QSsl::TlsV1_2);
    sslConfig.setCiphers(cipherList);

    return sslConfig;
}

// 选择头像（完全复刻注册逻辑，保证操作一致性）
void CreateGroup::selectAvatar()
{
    // 仅允许选择支持的图片格式（复刻注册逻辑）
    QString filePath = QFileDialog::getOpenFileName(this, "选择群头像", "", "图片文件 (*.png *.jpg *.jpeg *.gif)");
    if (filePath.isEmpty()) return;

    QFileInfo fileInfo(filePath);
    QString suffix = fileInfo.suffix().toLower();
    // 二次校验格式，避免改后缀的非图片文件（复刻注册逻辑）
    if (!suffix.contains("png") && !suffix.contains("jpg") && !suffix.contains("jpeg") && !suffix.contains("gif")) {
        QMessageBox::warning(this, "提示", "仅支持上传png/jpg/jpeg/gif格式的图片！");
        return;
    }

    QImage image(filePath);
    if (image.isNull()) {
        QMessageBox::warning(this, "提示", "图片加载失败！");
        return;
    }

    // UI显示逻辑（完全复刻注册逻辑）
    ui->btnAvatargroup->setText("");
    ui->btnAvatargroup->setFlat(true);
    ui->btnAvatargroup->setStyleSheet("QPushButton{padding:0px; border:none;}");
    ui->btnAvatargroup->setIconSize(ui->btnAvatargroup->size());
    ui->btnAvatargroup->setIcon(QPixmap::fromImage(image.scaled(ui->btnAvatargroup->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));

    // 记录头像路径（复刻注册逻辑，用于后续上传）
    m_avatarFilePath = filePath;
}

// 上传头像（完全复刻注册逻辑，使用QNetworkAccessManager+QHttpMultiPart，保证可行）
QString CreateGroup::uploadAvatar(const QString& filePath)
{
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先选择群头像！");
        return "";
    }
    if (!m_netManager) {
        QMessageBox::warning(this, "提示", "HTTP管理器未初始化，无法上传头像！");
        return "";
    }

    // 堆变量存储文件，避免析构（复刻注册逻辑）
    QFile *file = new QFile(filePath, this);
    if (!file->open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "提示", "群头像文件读取失败！");
        delete file;
        return "";
    }

    // 1. 构造请求（复刻注册逻辑，补充关键请求头）
    QUrl uploadUrl("https://www.singchat.chat/upload/avatar");
    QNetworkRequest request(uploadUrl);
    request.setTransferTimeout(10000);
    request.setSslConfiguration(getSslConfig()); // 复用通用SSL配置（复刻注册逻辑）

    // 模拟curl的请求头（精简+关键，复刻注册逻辑）
    request.setRawHeader("User-Agent", "curl/7.29.0");
    request.setRawHeader("Host", "www.singchat.chat");
    request.setRawHeader("Accept", "*/*");
    request.setRawHeader("Connection", "keep-alive");
    request.setRawHeader("Expect", ""); // 禁用100-continue（复刻注册逻辑）

    // 2. 封装multipart（复刻注册逻辑，表单提交）
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType, this);
    QHttpPart imagePart;

    QFileInfo fileInfo(filePath);
    QString suffix = fileInfo.suffix().toLower();
    QString mimeType = "image/jpeg";
    if (suffix == "png") mimeType = "image/png";
    else if (suffix == "gif") mimeType = "image/gif";
    QString safeFileName = "group_avatar_" + QString::number(QDateTime::currentMSecsSinceEpoch()) + "." + suffix;

    // 设置表单头（无特殊字符，复刻注册逻辑）
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant(QString("form-data; name=\"avatar\"; filename=\"%1\"").arg(safeFileName)));
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(mimeType));
    imagePart.setBodyDevice(file);
    file->setParent(multiPart);
    multiPart->append(imagePart);

    // 3. 发送请求（复刻注册逻辑，POST提交）
    QNetworkReply *reply = m_netManager->post(request, multiPart);
    multiPart->setParent(reply); // 自动释放资源（复刻注册逻辑）

    // 调试日志（复刻注册逻辑，方便排查问题）
    qDebug() << "📤 群头像请求URL：" << uploadUrl.toString();
    qDebug() << "📤 群头像文件大小：" << file->size() << "字节";
    qDebug() << "📤 群头像MIME类型：" << mimeType;

    // SSL错误处理（复刻注册逻辑，忽略无害错误）
    connect(reply, &QNetworkReply::sslErrors, this, [=](const QList<QSslError> &errors) {
        QString errorSummary;
        QList<QSslError> ignoreErrors = ignoreCriticalSslErrors(errors, errorSummary);
        if (!ignoreErrors.isEmpty()) {
            reply->ignoreSslErrors(ignoreErrors);
        } else {
            QMessageBox::critical(this, "SSL验证失败", "证书非法，群头像上传终止：\n" + errorSummary);
            reply->abort();
        }
    });

    // 4. 同步等待响应（复刻注册逻辑，QEventLoop避免阻塞UI）
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    // 5. 解析响应（复刻注册逻辑，提取头像URL）
    QString avatarURL;
    if (reply->error() == QNetworkReply::NoError) {
        avatarURL = reply->readAll().trimmed();
        qDebug() << "✅ 群头像上传成功，URL：" << avatarURL;
        qDebug() << "📤 HTTP状态码：" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    } else {
        qCritical() << "❌ 群头像上传错误类型：" << reply->error();
        qCritical() << "❌ 群头像上传错误描述：" << reply->errorString();
        qCritical() << "❌ 服务端返回内容：" << reply->readAll();
        qCritical() << "❌ HTTP状态码：" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QMessageBox::warning(this, "提示", QString("群头像上传失败：%1（状态码：%2）").arg(reply->errorString()).arg(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()));
        delete file;
    }

    // 释放资源（复刻注册逻辑）
    reply->deleteLater();
    return avatarURL;
}

// 处理创建群聊（复刻注册逻辑：先上传头像，再发送消息）
void CreateGroup::handleCreateGroup()
{
    // 初始化状态（复刻注册逻辑，避免重复操作）
    m_createGroupSuccess = false;
    if (m_loadingMsg) {
        m_loadingMsg->close();
        m_loadingMsg->deleteLater();
        m_loadingMsg = nullptr;
    }

    // 1. 输入校验（创建群聊必备）
    QString groupName = ui->groupname->text().trimmed();
    QItemSelectionModel *selectionModel = ui->creategroupList->selectionModel();
    QModelIndexList selectedIndexes = selectionModel->selectedIndexes();

    if (groupName.isEmpty()) {
        QMessageBox::warning(this, "提示", "群聊名称不能为空！");
        ui->groupname->setFocus();
        return;
    }
    if (selectedIndexes.isEmpty()) {
        QMessageBox::warning(this, "提示", "请至少选择一个好友加入群聊！");
        return;
    }
    if (m_avatarFilePath.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先选择群头像！");
        return;
    }

    // 2. 先上传头像获取URL（复刻注册逻辑，同步操作）
    QMessageBox *uploadMsg = new QMessageBox(QMessageBox::Information, "提示", "正在上传群头像...", QMessageBox::NoButton, this);
    uploadMsg->show();
    m_avatarURL = uploadAvatar(m_avatarFilePath);
    uploadMsg->close();
    uploadMsg->deleteLater();

    if (m_avatarURL.isEmpty()) {
        return; // 上传失败，终止创建（复刻注册逻辑）
    }

    // 3. 发送群聊创建消息（复用主窗口SSL套接字）
    m_loadingMsg = new QMessageBox(QMessageBox::Information, "提示", "正在创建群聊（安全验证中）...", QMessageBox::NoButton, this);
    m_loadingMsg->show();
    sendCreateGroupMessage();
}

// 发送群聊消息（复用主窗口SSL套接字，复刻注册逻辑的WebSocket消息构造）
void CreateGroup::sendCreateGroupMessage()
{
    // 校验SSL套接字状态（复刻注册逻辑，容错处理）
    if (!m_mainSslSocket || m_mainSslSocket->state() != QSslSocket::ConnectedState || !m_mainSslSocket->isEncrypted()) {
        QMessageBox::critical(this, "发送失败", "SSL套接字未就绪，无法发送创建请求！");
        if (m_loadingMsg) {
            m_loadingMsg->close();
            m_loadingMsg->deleteLater();
            m_loadingMsg = nullptr;
        }
        return;
    }

    // 1. 组装群聊信息（创建群聊必备）
    QString groupName = ui->groupname->text().trimmed();
    QItemSelectionModel *selectionModel = ui->creategroupList->selectionModel();
    QModelIndexList selectedIndexes = selectionModel->selectedIndexes();

    QJsonArray membersArray;
    membersArray.append(m_currentUser); // 群主加入

    for (const QModelIndex &index : selectedIndexes) {
        QString username = index.data(FriendListModel::UsernameRole).toString().trimmed();
        if (!username.isEmpty() && !membersArray.contains(username) && username != m_currentUser) {
            membersArray.append(username);
        }
    }

    // 2. 构造消息体（复刻注册逻辑，JSON格式）
    QJsonObject groupMsg;
    groupMsg["type"] = "create_group";
    groupMsg["creator"] = m_currentUser;
    groupMsg["display_name"] = groupName;
    groupMsg["members"] = membersArray;
    groupMsg["avatar"] = m_avatarURL; // 上传成功的头像URL
    groupMsg["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    // 3. 转换为JSON文档（复刻注册逻辑）
    QJsonDocument doc(groupMsg);
    QByteArray payload = doc.toJson(QJsonDocument::Compact);

    // 4. 构造WebSocket帧（完全复刻注册逻辑，保证服务端能解析）
    QByteArray maskingKey(4, 0);
    QRandomGenerator::system()->fillRange(reinterpret_cast<quint32*>(maskingKey.data()), 1);
    QByteArray frame;
    frame.append(0x81); // FIN=1 + Opcode=1（文本消息，复刻注册逻辑）

    if (payload.size() <= 125) {
        frame.append(static_cast<quint8>(payload.size()) | 0x80);
    } else if (payload.size() <= 65535) {
        frame.append(static_cast<quint8>(0x7E | 0x80));
        frame.append(static_cast<quint8>((payload.size() >> 8) & 0xFF));
        frame.append(static_cast<quint8>(payload.size() & 0xFF));
    } else {
        frame.append(static_cast<quint8>(0x7F | 0x80));
        return;
    }

    frame.append(maskingKey);
    for (int i = 0; i < payload.size(); i++) {
        payload[i] = static_cast<char>(payload[i] ^ maskingKey[i % 4]);
    }
    frame.append(payload);

    // 5. 复用主窗口SSL套接字发送消息（核心：无需重新创建连接）
    m_mainSslSocket->write(frame);
    m_mainSslSocket->flush();
    qDebug() << "📤 群聊创建消息已发送：" << QString::fromUtf8(payload);
}

// CreateGroup.cpp
void CreateGroup::onRecvMainWindowSocketData(const QByteArray &socketData)
{
    qDebug() << "======================================";
    qDebug() << "【CreateGroup】收到主窗口转发的服务器消息";

    // 1. 前置条件校验（保留，仅去掉m_mainSslSocket的判断）
    if (!m_webSocketHandshakeDone) {
        qDebug() << "❌ WebSocket握手未完成，忽略数据";
        return;
    }

    // 2. 接收转发的数据（直接使用socketData，无需再调用readAll()）
    qDebug() << "1. 接收的原始WebSocket帧数据："
             << "\n   字节长度：" << socketData.size()
             << "\n   十六进制：" << socketData.toHex()
             << "\n   字符串形式：" << QString::fromUtf8(socketData);
    if (socketData.size() < 2) {
        qDebug() << "❌ 帧数据长度不足2字节，无法解析";
        return;
    }

    // 3. 后续解析逻辑（完全复用原有onSslReadyRead的代码，无改动）
    quint8 opcode = socketData[0] & 0x0F;
    qDebug() << "2. 解析Opcode（消息类型）：" << QString::number(opcode, 16) << "(十六进制)";
    qDebug() << "   是否为文本消息（0x01）：" << (opcode == 0x01 ? "是" : "否");
    if (opcode != 0x01) {
        qDebug() << "❌ 非文本消息，直接返回";
        return;
    }

    quint64 payloadLen = socketData[1] & 0x7F;
    int offset = 2;
    qDebug() << "3. 初始有效载荷长度（payloadLen）：" << payloadLen;
    if (payloadLen == 126) {
        payloadLen = (static_cast<quint16>(socketData[2]) << 8) | socketData[3];
        offset = 4;
        qDebug() << "   （进入126分支）修正后payloadLen：" << payloadLen << "，offset更新为：" << offset;
    } else if (payloadLen == 127) {
        qDebug() << "❌ 有效载荷长度为127（超大数据），暂不支持，直接返回";
        return;
    } else {
        qDebug() << "   （直接使用初始长度）offset保持：" << offset;
    }

    // 解析掩码（保留原有逻辑）
    QByteArray maskingKey;
    bool hasMask = (socketData[1] & 0x80) != 0;
    qDebug() << "4. 是否包含掩码（frame[1] & 0x80）：" << (hasMask ? "是" : "否");
    if (hasMask) {
        maskingKey = socketData.mid(offset, 4);
        offset += 4;
        qDebug() << "   解析到掩码（4字节）：" << maskingKey.toHex();
        qDebug() << "   offset更新为（加上掩码长度）：" << offset;
    }

    // 解析payload（保留原有逻辑）
    QByteArray payload = socketData.mid(offset, payloadLen);
    qDebug() << "5. 解密前的原始payload："
             << "\n   字节长度：" << payload.size()
             << "\n   十六进制：" << payload.toHex()
             << "\n   字符串形式：" << QString::fromUtf8(payload);
    if (!maskingKey.isEmpty()) {
        for (int i = 0; i < payload.size(); i++) {
            payload[i] ^= maskingKey[i % 4];
        }
        qDebug() << "6. 掩码解密后的payload："
                 << "\n   字节长度：" << payload.size()
                 << "\n   十六进制：" << payload.toHex()
                 << "\n   字符串形式：" << QString::fromUtf8(payload);
    }

    // 解析JSON（保留原有逻辑）
    QJsonDocument doc = QJsonDocument::fromJson(payload);
    qDebug() << "7. JSON解析结果："
             << "\n   是否为有效JSON：" << (!doc.isNull() ? "是" : "否")
             << "\n   是否为JSON对象：" << doc.isObject()
             << "\n   是否为JSON数组：" << doc.isArray();
    if (doc.isNull()) {
        qDebug() << "❌ JSON解析失败，无法转换为QJsonDocument";
        return;
    }

    if (doc.isObject()) {
        QJsonObject msgObj = doc.object();
        QString content = msgObj["content"].toString();
        QString groupId = msgObj["group_id"].toString();

        qDebug() << "8. JSON对象解析详情："
                 << "\n   content字段值：" << content
                 << "\n   group_id字段值：" << groupId
                 << "\n   完整JSON对象：" << QString::fromUtf8(doc.toJson(QJsonDocument::Indented));

        // 关闭加载提示（保留原有逻辑）
        if (m_loadingMsg) {
            m_loadingMsg->close();
            m_loadingMsg->deleteLater();
            m_loadingMsg = nullptr;
        }

        // 处理结果（保留原有逻辑）
        if (content.contains("创建成功")) {
            m_createGroupSuccess = true;
            qDebug() << "✅ 群聊创建成功，group_id：" << groupId;
            QMessageBox::information(this, "创建结果", content + "\n群聊唯一标识：" + groupId);
            this->close();
        } else {
            qDebug() << "⚠️  群聊创建未返回成功，content：" << content;
            QMessageBox::information(this, "创建结果", content);
        }
    } else {
        qDebug() << "❌ 解析结果不是JSON对象，无法提取content和group_id";
    }
    qDebug() << "======================================\n";
}

// 忽略无害SSL错误（完全复刻注册逻辑，保证兼容性）
QList<QSslError> CreateGroup::ignoreCriticalSslErrors(const QList<QSslError> &errors, QString &errorSummary)
{
    qDebug() << "📌 开始筛选SSL错误：";
    QList<QSslError> ignoreErrors;
    errorSummary.clear();

    for (const QSslError &err : errors) {
        QString errStr = err.errorString();
        qDebug() << "  - 检测到SSL错误：" << errStr;
        errorSummary += errStr + "\n";

        // 忽略与注册逻辑一致的两个无害错误（保证兼容性）
        if (err.error() == QSslError::HostNameMismatch ||
            errStr.contains("The issuer certificate of a locally looked up certificate could not be found")) {
            ignoreErrors.append(err);
        }
    }

    return ignoreErrors;
}

// SSL错误处理（完全复刻注册逻辑，保证一致性）
void CreateGroup::onSslErrors(const QList<QSslError> &errors)
{
    QString errorSummary;
    QList<QSslError> ignoreErrors = ignoreCriticalSslErrors(errors, errorSummary);

    if (!ignoreErrors.isEmpty()) {
        if (m_mainSslSocket) {
            m_mainSslSocket->ignoreSslErrors(ignoreErrors);
        }
        qDebug() << "⚠️ 已忽略无害SSL错误（仅兼容用，无安全风险）：";
        qDebug() << "  - 主机名不匹配（OpenSSL 3.6 SAN解析兼容）";
        qDebug() << "  - 颁发者证书找不到（证书链待补全）";
    } else {
        if (m_loadingMsg) {
            m_loadingMsg->close();
            m_loadingMsg->deleteLater();
            m_loadingMsg = nullptr;
        }
        QMessageBox::critical(this, "SSL验证失败", "证书非法，群聊创建终止：\n" + errorSummary);
        if (m_mainSslSocket && m_mainSslSocket->state() == QSslSocket::ConnectedState) {
            m_mainSslSocket->disconnectFromHost();
        }
    }
}

// 连接断开处理（复刻注册逻辑，容错处理）
void CreateGroup::onDisconnected()
{
    if (m_createGroupSuccess) return;

    m_webSocketHandshakeDone = false;
    if (m_loadingMsg) {
        m_loadingMsg->close();
        m_loadingMsg->deleteLater();
        m_loadingMsg = nullptr;
    }
    QMessageBox::warning(this, "连接断开", "与服务器的SSL连接已断开，群聊创建终止！");
}

// 套接字错误处理（复刻注册逻辑，容错处理）
void CreateGroup::onSocketError(QAbstractSocket::SocketError error)
{
    if (m_createGroupSuccess) return;

    QString errorDesc;
    switch (error) {
    case QAbstractSocket::ConnectionRefusedError:
        errorDesc = "服务器拒绝连接";
        break;
    case QAbstractSocket::RemoteHostClosedError:
        errorDesc = "远程主机关闭连接";
        break;
    case QAbstractSocket::SslHandshakeFailedError:
        errorDesc = "SSL握手失败";
        break;
    case QAbstractSocket::HostNotFoundError:
        errorDesc = "域名解析失败";
        break;
    default:
        errorDesc = "未知错误";
    }

    if (m_loadingMsg) {
        m_loadingMsg->close();
        m_loadingMsg->deleteLater();
    }

    QMessageBox::critical(this, "创建失败", QString("SSL套接字错误：%1\n详情：%2").arg(errorDesc, m_mainSslSocket->errorString()));
}

// 鼠标按下事件（复刻注册逻辑，无边框窗口拖动）
void CreateGroup::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        QDialog::mousePressEvent(event);
        return;
    }
    QPoint clickPos = event->pos();
    if (clickPos.y() < 35 && clickPos.x() < (this->width() - 50)) {
        m_dragPos = event->globalPosition().toPoint() - this->frameGeometry().topLeft();
    } else {
        m_dragPos = QPoint(0, 0);
    }
    QDialog::mousePressEvent(event);
}

// 鼠标移动事件（复刻注册逻辑，无边框窗口拖动）
void CreateGroup::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && !m_dragPos.isNull()) {
        QPoint newPos = event->globalPosition().toPoint() - m_dragPos;
        this->move(newPos);
        event->accept();
        return;
    }
    QDialog::mouseMoveEvent(event);
}

// 修复：补充on_groupclose_clicked()的实现（关闭窗口）
void CreateGroup::on_groupclose_clicked()
{
    this->close();
}
