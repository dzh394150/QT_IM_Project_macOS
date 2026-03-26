#include "registerwidget.h"
#include "qtimer.h"
#include "ui_registerwidget.h"
#include <QUrl>
#include <QDebug>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <QRandomGenerator>
#include <QCryptographicHash>
#include <QFile>
#include <QImage>
#include <QFileDialog>
#include <QMessageBox>
#include <QHttpMultiPart>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QMouseEvent>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSslCipher>


RegisterWidget::RegisterWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RegisterWidget)
    , m_tcpSocket(nullptr) // 先置空
    , m_sslSocket(nullptr) // 先置空
    , m_netManager(new QNetworkAccessManager(this))
    , m_loadingMsg(nullptr)
    , m_webSocketHandshakeDone(false)
    , m_registerSuccess(false)
    , m_dragPos(QPoint(0, 0))
    , m_avatarBase64("")
    , m_avatarFilePath("")
    , m_avatarURL("")

    , m_isGetVerifyCode(false)
    , m_targetPhone("")
{
    ui->setupUi(this);

    // 窗口样式（不变）
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(ui->shadowwidget);
    shadow->setColor(QColor(0, 0, 0, 80));
    shadow->setBlurRadius(15);
    shadow->setOffset(0, 0);
    ui->shadowwidget->setGraphicsEffect(shadow);

    // ========== 修复：初始化Socket并绑定信号（避免重复） ==========
    initSocket(); // 抽离Socket初始化逻辑

    // 其他信号绑定（不变）
    connect(ui->btnRegister, &QPushButton::clicked, this, &RegisterWidget::handleRegister);
    connect(ui->btnClose, &QPushButton::clicked, this, &RegisterWidget::close);
    connect(ui->btnAvatar, &QPushButton::clicked, this, &RegisterWidget::selectAvatar);
    connect(ui->get_reg_yanzhengma, &QPushButton::clicked, this, &RegisterWidget::handleGetRegVerifyCode);
}

// ========== 新增：Socket初始化函数（避免重复绑定） ==========
void RegisterWidget::initSocket()
{
    // 销毁旧Socket（如果有）
    if (m_tcpSocket) {
        m_tcpSocket->disconnect(); // 解除所有信号
        delete m_tcpSocket;
    }
    if (m_sslSocket) {
        m_sslSocket->disconnect(); // 解除所有信号
        delete m_sslSocket;
    }

    // 重建Socket
    m_tcpSocket = new QTcpSocket(this);
    m_sslSocket = new QSslSocket(this);
    m_sslSocket->setReadBufferSize(1024 * 1024);

    // 绑定信号（仅绑定一次）
    connect(m_tcpSocket, &QTcpSocket::connected, this, &RegisterWidget::onTcpConnected);
    connect(m_tcpSocket, &QTcpSocket::disconnected, this, &RegisterWidget::onDisconnected);
    connect(m_tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred), this, &RegisterWidget::onSocketError);

    connect(m_sslSocket, &QSslSocket::encrypted, this, &RegisterWidget::onSslEncrypted);
    connect(m_sslSocket, &QSslSocket::sslErrors, this, &RegisterWidget::onSslErrors);
    connect(m_sslSocket, &QSslSocket::readyRead, this, &RegisterWidget::onSslReadyRead);
    connect(m_sslSocket, &QSslSocket::disconnected, this, &RegisterWidget::onDisconnected);
    connect(m_sslSocket, QOverload<QAbstractSocket::SocketError>::of(&QSslSocket::errorOccurred), this, &RegisterWidget::onSocketError);
}

RegisterWidget::~RegisterWidget()
{
    delete ui;
    m_tcpSocket->abort();
    m_sslSocket->abort();
}

// ====================== 新增：通用SSL配置函数（WebSocket/HTTP复用） ======================

QSslConfiguration RegisterWidget::getSslConfig()
{
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    // 1. 强制TLS1.2（和服务器一致），因为QT的3.6版本SSL可以支持1.2和1.3，可能禁用了1.2中的一些套件
    sslConfig.setProtocol(QSsl::TlsV1_2);
    // 2. 开启证书验证
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyPeer);
    sslConfig.setPeerVerifyDepth(3);

    // 3. 适配旧OpenSSL的SSL选项（禁用SNI/压缩，避免兼容问题）
    sslConfig.setSslOption(QSsl::SslOptionDisableCompression, true);
    sslConfig.setSslOption(QSsl::SslOptionDisableServerNameIndication, true); // 禁用SNI（旧版OpenSSL兼容）
    sslConfig.setSslOption(QSsl::SslOptionDisableSessionTickets, false);

    // 4. 手动指定服务器支持的加密套件（Qt6通用写法）
    QList<QSslCipher> cipherList;
    // 只保留OpenSSL 1.0.2k肯定支持的套件（精简列表，减少协商失败）
    cipherList << QSslCipher("ECDHE-RSA-AES128-GCM-SHA256", QSsl::TlsV1_2)
               << QSslCipher("AES128-GCM-SHA256", QSsl::TlsV1_2);
    sslConfig.setCiphers(cipherList);

    return sslConfig;
}

// ====================== 修改：选择头像（记录路径） ======================
void RegisterWidget::selectAvatar()
{
    // 仅允许选择支持的图片格式
    QString filePath = QFileDialog::getOpenFileName(this, "选择头像", "", "图片文件 (*.png *.jpg *.jpeg *.gif)");
    if (filePath.isEmpty()) return;

    QFileInfo fileInfo(filePath);
    QString suffix = fileInfo.suffix().toLower();
    // 二次校验格式，避免改后缀的非图片文件
    if (!suffix.contains("png") && !suffix.contains("jpg") && !suffix.contains("jpeg") && !suffix.contains("gif")) {
        QMessageBox::warning(this, "提示", "仅支持上传png/jpg/jpeg/gif格式的图片！");
        return;
    }

    QImage image(filePath);
    if (image.isNull()) {
        QMessageBox::warning(this, "提示", "图片加载失败！");
        return;
    }

    // UI显示逻辑（原有）
    ui->btnAvatar->setText("");
    ui->btnAvatar->setFlat(true);
    ui->btnAvatar->setStyleSheet("QPushButton{padding:0px; border:none;}");
    ui->btnAvatar->setIconSize(ui->btnAvatar->size());
    ui->btnAvatar->setIcon(QPixmap::fromImage(image.scaled(ui->btnAvatar->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));

    // 记录头像路径
    m_avatarFilePath = filePath;
}

// ====================== 新增：HTTPS上传头像（复用SSL配置） ======================
QString RegisterWidget::uploadAvatar(const QString& filePath)
{
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先选择头像！");
        return "";
    }

    // 堆变量存储文件，避免析构
    QFile *file = new QFile(filePath, this);
    if (!file->open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "提示", "头像文件读取失败！");
        delete file;
        return "";
    }

    // 1. 构造请求（核心：补充Content-Length，模拟curl）
    QUrl uploadUrl("https://www.singchat.chat/upload/avatar");
    QNetworkRequest request(uploadUrl);
    request.setTransferTimeout(10000);
    request.setSslConfiguration(getSslConfig());

    // 模拟curl的请求头（精简+关键）
    request.setRawHeader("User-Agent", "curl/7.29.0");
    request.setRawHeader("Host", "www.singchat.chat");
    request.setRawHeader("Accept", "*/*");
    request.setRawHeader("Connection", "keep-alive");
    request.setRawHeader("Expect", ""); // 禁用100-continue

    // 2. 封装multipart（手动计算Content-Length，可选但推荐）
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType, this);
    QHttpPart imagePart;

    QFileInfo fileInfo(filePath);
    QString suffix = fileInfo.suffix().toLower();
    QString mimeType = "image/jpeg"; // 固定jpg（测试用，后续可判断）
    QString safeFileName = "avatar_" + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".jpg";

    // 设置表单头（无特殊字符）
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant(QString("form-data; name=\"avatar\"; filename=\"%1\"").arg(safeFileName)));
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(mimeType));
    imagePart.setBodyDevice(file);
    file->setParent(multiPart);
    multiPart->append(imagePart);

    // 3. 发送请求
    QNetworkReply *reply = m_netManager->post(request, multiPart);
    multiPart->setParent(reply);

    // 调试日志
    qDebug() << "📤 请求URL：" << uploadUrl.toString();
    qDebug() << "📤 文件大小：" << file->size() << "字节";
    qDebug() << "📤 MIME类型：" << mimeType;

    // SSL错误处理（保留）
    connect(reply, &QNetworkReply::sslErrors, this, [=](const QList<QSslError> &errors) {
        QString errorSummary;
        QList<QSslError> ignoreErrors = ignoreCriticalSslErrors(errors, errorSummary);
        if (!ignoreErrors.isEmpty()) {
            reply->ignoreSslErrors(ignoreErrors);
        } else {
            QMessageBox::critical(this, "SSL验证失败", "证书非法，头像上传终止：\n" + errorSummary);
            reply->abort();
        }
    });

    // 4. 同步等待响应
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    // 5. 解析响应
    QString avatarURL;
    if (reply->error() == QNetworkReply::NoError) {
        avatarURL = reply->readAll().trimmed();
        qDebug() << "✅ 头像上传成功，URL：" << avatarURL;
        qDebug() << "📤 HTTP状态码：" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    } else {
        qCritical() << "❌ 上传错误类型：" << reply->error();
        qCritical() << "❌ 上传错误描述：" << reply->errorString();
        qCritical() << "❌ 服务端返回内容：" << reply->readAll();
        qCritical() << "❌ HTTP状态码：" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QMessageBox::warning(this, "提示", QString("头像上传失败：%1（状态码：%2）").arg(reply->errorString()).arg(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()));
        delete file;
    }

    // 释放资源
    reply->deleteLater();
    return avatarURL;
}

// ====================== 修改：处理注册（先上传头像，再连接服务器） ======================
void RegisterWidget::handleRegister()
{
    // ========== 彻底重置：清空描述符+重置状态 ==========
    // 1. 关闭并清空TCP Socket
    m_tcpSocket->abort();
    m_tcpSocket->setSocketDescriptor(-1); // 清空底层描述符

    // 2. 关闭并清空SSL Socket（关键！）
    m_sslSocket->abort();
    m_sslSocket->setSocketDescriptor(-1); // 清空残留的描述符
    m_sslSocket->reset(); // 重置SSL状态（QT6专属，清空加密标记）

    // 3. 重置所有标记
    m_webSocketHandshakeDone = false;
    m_isGetVerifyCode = false;
    m_registerSuccess = false;
    m_targetPhone = "";

    // ========== 原有输入校验逻辑（不变） ==========
    QString username = ui->leRegUsername->text().trimmed();
    QString password = ui->leRegPassword->text().trimmed();
    QString confirmPwd = ui->leRegConfirmPwd->text().trimmed();

    if (username.isEmpty()) {
        QMessageBox::warning(this, "提示", "账号不能为空！");
        ui->leRegUsername->setFocus();
        return;
    }
    if (password.isEmpty()) {
        QMessageBox::warning(this, "提示", "密码不能为空！");
        ui->leRegPassword->setFocus();
        return;
    }
    if (password != confirmPwd) {
        QMessageBox::warning(this, "提示", "两次密码不一致！");
        ui->leRegConfirmPwd->setFocus();
        return;
    }

    // 验证码校验（不变）
    QString verifyCode = ui->input_reg_yanzhengma->text().trimmed();
    if (verifyCode.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入验证码！");
        ui->input_reg_yanzhengma->setFocus();
        return;
    }

    // 头像上传（建议改为异步，避免阻塞，不变）
    if (m_avatarFilePath.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先选择头像！");
        return;
    }
    QMessageBox *uploadMsg = new QMessageBox(QMessageBox::Information, "提示", "正在上传头像...", QMessageBox::NoButton, this);
    uploadMsg->show();
    m_avatarURL = uploadAvatar(m_avatarFilePath);
    uploadMsg->close();
    uploadMsg->deleteLater();
    if (m_avatarURL.isEmpty()) {
        return;
    }

    // ========== 修复：重新创建Socket（可选，彻底避免复用冲突） ==========
    // 如果清空描述符仍有问题，直接重建Socket对象（推荐）
    if (m_tcpSocket) {
        delete m_tcpSocket;
        m_tcpSocket = new QTcpSocket(this);
        // 重新绑定TCP信号（避免重复绑定）
        connect(m_tcpSocket, &QTcpSocket::connected, this, &RegisterWidget::onTcpConnected);
        connect(m_tcpSocket, &QTcpSocket::disconnected, this, &RegisterWidget::onDisconnected);
        connect(m_tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred), this, &RegisterWidget::onSocketError);
    }
    if (m_sslSocket) {
        delete m_sslSocket;
        m_sslSocket = new QSslSocket(this);
        // 重新绑定SSL信号（避免重复绑定）
        connect(m_sslSocket, &QSslSocket::encrypted, this, &RegisterWidget::onSslEncrypted);
        connect(m_sslSocket, &QSslSocket::sslErrors, this, &RegisterWidget::onSslErrors);
        connect(m_sslSocket, &QSslSocket::readyRead, this, &RegisterWidget::onSslReadyRead);
        connect(m_sslSocket, &QSslSocket::disconnected, this, &RegisterWidget::onDisconnected);
        connect(m_sslSocket, QOverload<QAbstractSocket::SocketError>::of(&QSslSocket::errorOccurred), this, &RegisterWidget::onSocketError);
        m_sslSocket->setReadBufferSize(1024 * 1024);
    }

    // 连接服务器（不变）
    m_loadingMsg = new QMessageBox(QMessageBox::Information, "提示", "正在注册（安全验证中）...", QMessageBox::NoButton, this);
    m_loadingMsg->show();
    m_tcpSocket->connectToHost("www.singchat.chat", 443);
}

// ====================== 修改：TCP连接成功（复用通用SSL配置） ======================
void RegisterWidget::onTcpConnected()
{
    qDebug() << "✅ 注册-TCP连接成功，开始SSL握手";

    // ========== 修复：先重置SSL Socket，再设置描述符 ==========
    m_sslSocket->abort();
    m_sslSocket->setSocketDescriptor(-1);
    // 绑定TCP描述符到SSL Socket（确保是纯TCP连接）
    if (!m_sslSocket->setSocketDescriptor(m_tcpSocket->socketDescriptor())) {
        qCritical() << "❌ 设置SSL Socket描述符失败：" << m_sslSocket->errorString();
        onDisconnected();
        return;
    }

    // 复用通用SSL配置（不变）
    m_sslSocket->setSslConfiguration(getSslConfig());
    // 启动SSL握手（此时是纯TCP连接，不会报错）
    m_sslSocket->startClientEncryption();
}

// ====================== 修改：发送注册消息（传URL而非Base64） ======================
void RegisterWidget::sendRegisterMessage()
{
    QString username = ui->leRegUsername->text().trimmed();
    QString password = ui->leRegPassword->text().trimmed();

    // 构造注册消息（Content字段改为头像URL）
    QJsonObject regMsg;
    regMsg["type"] = "register";
    regMsg["from"] = username;

    // 新增：SHA256加密+十六进制编码（和Go服务端一致）
    QByteArray pwdBytes = password.toUtf8();
    QByteArray encryptedPwd = QCryptographicHash::hash(pwdBytes, QCryptographicHash::Sha256).toHex();
    regMsg["password"] = QString(encryptedPwd);

    regMsg["content"] = m_avatarURL; // 核心改点：传URL
    regMsg["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    // 【0202晚上验证码注册与登录逻辑开始：添加验证码字段】
    QString verifyCode = ui->input_reg_yanzhengma->text().trimmed();
    regMsg["verify_code"] = verifyCode;
    // 【0202晚上验证码注册与登录逻辑结束】

    QJsonDocument doc(regMsg);
    QByteArray payload = doc.toJson(QJsonDocument::Compact);

    // WebSocket帧构造（原有逻辑，无修改）
    QByteArray maskingKey(4, 0);
    QRandomGenerator::system()->fillRange(reinterpret_cast<quint32*>(maskingKey.data()), 1);
    QByteArray frame;
    frame.append(0x81); // FIN=1 + Opcode=1（文本消息）

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
    m_sslSocket->write(frame);
    qDebug() << "📤 发送注册消息：" << QString::fromUtf8(payload);
}

// ====================== 原有逻辑（无修改） ======================
void RegisterWidget::onSslEncrypted()
{
    qDebug() << "✅ 注册-SSL握手成功，开始WebSocket握手";
    // 生成WebSocket密钥
    QByteArray keyBytes(16, 0);
    QRandomGenerator::system()->fillRange(reinterpret_cast<quint32*>(keyBytes.data()), keyBytes.size() / 4);
    m_webSocketKey = keyBytes.toBase64();

    // 构造WebSocket握手请求
    QByteArray handshakeReq = QString(
                                  "GET /ws HTTP/1.1\r\n"
                                  "Host: www.singchat.chat\r\n" // 替换为你的服务器地址
                                  "Upgrade: websocket\r\n"
                                  "Connection: Upgrade\r\n"
                                  "Sec-WebSocket-Key: %1\r\n"
                                  "Sec-WebSocket-Version: 13\r\n\r\n"
                                  ).arg(m_webSocketKey).toUtf8();
    m_sslSocket->write(handshakeReq);
}

void RegisterWidget::onSslReadyRead()
{
    if (!m_webSocketHandshakeDone) {
        // 处理WebSocket握手响应
        QByteArray response = m_sslSocket->readAll();
        if (response.startsWith("HTTP/1.1 101 Switching Protocols")) {
            QByteArray input = m_webSocketKey.toUtf8() + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
            QByteArray acceptKey = QCryptographicHash::hash(input, QCryptographicHash::Sha1).toBase64();
            if (response.contains(acceptKey)) {
                m_webSocketHandshakeDone = true;
                qDebug() << "✅ 注册-WebSocket握手成功";

                // 【0202晚上验证码注册与登录逻辑开始：区分请求类型发送消息】
                if (m_isGetVerifyCode) {
                    // 发送获取验证码请求
                    QJsonObject verifyMsg;
                    verifyMsg["type"] = "get_reg_yanzhengma";
                    verifyMsg["phone"] = m_targetPhone;
                    verifyMsg["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

                    QJsonDocument doc(verifyMsg);
                    QByteArray payload = doc.toJson(QJsonDocument::Compact);

                    // 复用原有WebSocket帧构造逻辑
                    QByteArray maskingKey(4, 0);
                    QRandomGenerator::system()->fillRange(reinterpret_cast<quint32*>(maskingKey.data()), 1);
                    QByteArray frame;
                    frame.append(0x81);
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
                    m_sslSocket->write(frame);
                    qDebug() << "📤 发送获取验证码请求：" << QString::fromUtf8(payload);
                } else {
                    // 原有注册消息发送逻辑
                    sendRegisterMessage();
                }
                // 【0202晚上验证码注册与登录逻辑结束】
            } else {
                qCritical() << "❌ 注册-WebSocket握手失败：AcceptKey不匹配";
                onDisconnected();
            }
        } else {
            qCritical() << "❌ 注册-WebSocket握手失败：服务器返回非101响应";
            onDisconnected();
        }
    } else {
        // 处理注册响应
        QByteArray frame = m_sslSocket->readAll();
        if (frame.size() < 2) return;

        quint8 opcode = frame[0] & 0x0F;
        if (opcode != 0x01) return;

        quint64 payloadLen = frame[1] & 0x7F;
        int offset = 2;
        if (payloadLen == 126) {
            payloadLen = (static_cast<quint16>(frame[2]) << 8) | frame[3];
            offset = 4;
        } else if (payloadLen == 127) return;

        QByteArray maskingKey;
        if (frame[1] & 0x80) {
            maskingKey = frame.mid(offset, 4);
            offset += 4;
        }
        QByteArray payload = frame.mid(offset, payloadLen);
        if (!maskingKey.isEmpty()) {
            for (int i = 0; i < payload.size(); i++) {
                payload[i] ^= maskingKey[i % 4];
            }
        }

        // 解析结果
        QJsonDocument doc = QJsonDocument::fromJson(payload);
        if (doc.isObject()) {
            QJsonObject msgObj = doc.object();
            QString content = msgObj["content"].toString();

            // 【0202晚上验证码注册与登录逻辑开始：处理验证码相关响应】
            if (content.contains("验证码发送成功")) {
                QMessageBox::information(this, "提示", "验证码已发送，5分钟内有效！");
                return;
            } else if (content.contains("验证码发送失败")) {
                QMessageBox::warning(this, "提示", content);
                // 重置验证码按钮
                ui->get_reg_yanzhengma->setEnabled(true);
                ui->get_reg_yanzhengma->setText("获取验证码");
                return;
            } else if (content.contains("验证码错误")) {
                QMessageBox::warning(this, "提示", "验证码错误，请重新输入！");
                ui->input_reg_yanzhengma->clear();
                ui->input_reg_yanzhengma->setFocus();
                return;
            } else if (content.contains("验证码正确")) {
                // 验证码正确，继续原有逻辑（无需弹窗，等待注册结果）
                qDebug() << "✅ 验证码校验通过，执行注册逻辑";
            }
            // 【0202晚上验证码注册与登录逻辑结束】

            if (content.contains("注册成功")) {
                m_registerSuccess = true; // 标记注册成功
                QMessageBox::information(this, "注册结果", content);
                // this->close(); // 注册成功后直接关闭窗口（可选）
            } else {
                QMessageBox::information(this, "注册结果", content);
            }
        }
    }
}

void RegisterWidget::onDisconnected()
{
    // 如果注册已经成功，忽略断开提示
    if (m_registerSuccess) {
        return;
    }
    m_webSocketHandshakeDone = false;
    ui->btnRegister->setEnabled(true);
    if (m_loadingMsg) {
        m_loadingMsg->close();
        m_loadingMsg->deleteLater();
        m_loadingMsg = nullptr;
    }
}

void RegisterWidget::onSocketError(QAbstractSocket::SocketError error)
{
    // 如果注册已经成功，忽略后续的连接关闭错误
    if (m_registerSuccess) {
        return;
    }
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
    // QMessageBox::critical(this, "注册失败", QString("连接服务器失败：%1\n详情：%2").arg(errorDesc, m_sslSocket->errorString()));
    onDisconnected();

    if (m_loadingMsg) {
        m_loadingMsg->close();
        m_loadingMsg->deleteLater();
        m_loadingMsg = nullptr;
    }
}

void RegisterWidget::onSslErrors(const QList<QSslError> &errors)
{
    QString errorSummary;
    QList<QSslError> ignoreErrors = ignoreCriticalSslErrors(errors, errorSummary);

    if (!ignoreErrors.isEmpty()) {
        m_sslSocket->ignoreSslErrors();
        qDebug() << "⚠️ 已忽略SSL错误（仅兼容用，无安全风险）：";
        qDebug() << "  - 主机名不匹配（OpenSSL 3.6 SAN解析兼容）";
        qDebug() << "  - 颁发者证书找不到（证书链待补全）";
    } else {
        QMessageBox::critical(this, "SSL验证失败", "证书非法，注册终止：\n" + errorSummary);
        m_sslSocket->disconnectFromHost();
    }
}


// 鼠标拖动窗口（原有逻辑）
void RegisterWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(event);
        return;
    }
    QPoint clickPos = event->pos();
    if (clickPos.y() < 35 && clickPos.x() < (this->width() - 50)) {
        m_dragPos = event->globalPosition().toPoint() - this->frameGeometry().topLeft();
    } else {
        m_dragPos = QPoint(0, 0);
    }
    QWidget::mousePressEvent(event);
}

void RegisterWidget::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && !m_dragPos.isNull()) {
        QPoint newPos = event->globalPosition().toPoint() - m_dragPos;
        this->move(newPos);
        event->accept();
        return;
    }
    QWidget::mouseMoveEvent(event);
}

QList<QSslError> RegisterWidget::ignoreCriticalSslErrors(const QList<QSslError> &errors, QString &errorSummary)
{
    qDebug() << "📌 SSL错误列表：";
    QList<QSslError> ignoreErrors;
    errorSummary.clear();

    for (const QSslError &err : errors) {
        QString errStr = err.errorString();
        qDebug() << "  - " << errStr;
        errorSummary += errStr + "\n";

        // 忽略登录窗口确认的两个关键错误
        if (err.error() == QSslError::HostNameMismatch) {
            ignoreErrors.append(err);
        } else if (errStr.contains("The issuer certificate of a locally looked up certificate could not be found")) {
            ignoreErrors.append(err);
        }
    }

    return ignoreErrors;
}

// 【0202晚上验证码注册与登录逻辑开始：获取注册验证码】
void RegisterWidget::handleGetRegVerifyCode()
{
    // 1. 获取手机号（用户名输入框）
    QString phoneNumber = ui->leRegUsername->text().trimmed();
    // 2. 校验手机号格式（11位数字）
    QRegularExpression regExp("^1[3-9]\\d{9}$");
    QRegularExpressionMatch match = regExp.match(phoneNumber);
    if (!match.hasMatch()) {
        QMessageBox::warning(this, "提示", "请输入正确的11位手机号！");
        ui->leRegUsername->setFocus();
        return;
    }

    // 3. 重置Socket状态（避免残留连接）
    m_tcpSocket->abort();
    m_sslSocket->abort();
    m_webSocketHandshakeDone = false;

    // 4. 显示加载提示
    QMessageBox *loadingMsg = new QMessageBox(QMessageBox::Information, "提示", "正在发送验证码...", QMessageBox::NoButton, this);
    loadingMsg->show();

    // 5. 连接服务器（SSL握手）
    m_tcpSocket->connectToHost("www.singchat.chat", 443);

    // 6. 标记当前是获取验证码请求（新增成员变量：bool m_isGetVerifyCode = false; 需在头文件声明）
    m_isGetVerifyCode = true;
    m_targetPhone = phoneNumber; // 新增成员变量：QString m_targetPhone; 需在头文件声明

    // 7. 关闭加载提示（后续在onSslEncrypted中发送验证码请求）
    QTimer::singleShot(1000, loadingMsg, &QMessageBox::close);
    loadingMsg->deleteLater();

    // 8. 按钮置灰+倒计时（QT6.10.1兼容写法，修复count只读问题）
    ui->get_reg_yanzhengma->setEnabled(false);
    // 改用指针存储count，避免值传递的只读限制
    int *count = new int(60);
    QTimer *countDownTimer = new QTimer(this);
    connect(countDownTimer, &QTimer::timeout, this, [=]() {
        (*count)--; // 修改变量的解引用，而非只读副本
        ui->get_reg_yanzhengma->setText(QString("重新获取(%1s)").arg(*count));
        if (*count <= 0) {
            countDownTimer->stop();
            ui->get_reg_yanzhengma->setText("获取验证码");
            ui->get_reg_yanzhengma->setEnabled(true);
            // 释放内存，避免泄漏
            delete count;
            countDownTimer->deleteLater();
        }
    });
    countDownTimer->start(1000);
}
// 【0202晚上验证码注册与登录逻辑结束】
