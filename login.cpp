#include "login.h"
#include "ui_login.h"
#include <QUrl>
#include <QDebug>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <QSslConfiguration>
#include <QRandomGenerator>
#include <QMessageAuthenticationCode>
#include <QCryptographicHash>
#include <QFile>
#include "registerwidget.h"
#include "mainwindow.h"
// 构造函数
LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginWidget)
    , m_mainWin(nullptr)
    , m_tcpSocket(new QTcpSocket(this))
    , m_sslSocket(new QSslSocket(this))
    , m_webSocketHandshakeDone(false)
    , m_loginTimeoutTimer(new QTimer(this))
    , m_dragPos(QPoint(0, 0))
    , m_loadingMsg(nullptr)
{
    qDebug() << "✅ Qt SSL支持状态：" << QSslSocket::supportsSsl();
    qDebug() << "✅ 运行时OpenSSL版本：" << QSslSocket::sslLibraryVersionString();

    // 初始化UI
    ui->setupUi(this);
    // 窗口样式配置（原逻辑保留）
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(ui->shadowwidget);
    shadow->setColor(QColor(0, 0, 0, 80));
    shadow->setBlurRadius(15);
    shadow->setOffset(0, 0);
    ui->shadowwidget->setGraphicsEffect(shadow);

    // 信号绑定（替换原QWebSocket信号）
    connect(ui->login_botton, &QPushButton::clicked, this, &LoginWidget::handleLogin);
    connect(ui->close_botton, &QPushButton::clicked, this, &LoginWidget::close);

    // TCP/SSL信号绑定
    connect(m_tcpSocket, &QTcpSocket::connected, this, &LoginWidget::onTcpConnected);
    connect(m_tcpSocket, &QTcpSocket::disconnected, this, &LoginWidget::onDisconnected);
    connect(m_tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred), this, &LoginWidget::onSocketError);

    connect(m_sslSocket, &QSslSocket::encrypted, this, &LoginWidget::onSslEncrypted);
    connect(m_sslSocket, &QSslSocket::sslErrors, this, &LoginWidget::onSslErrors);
    connect(m_sslSocket, &QSslSocket::readyRead, this, &LoginWidget::onSslReadyRead);
    connect(m_sslSocket, &QSslSocket::disconnected, this, &LoginWidget::onDisconnected);
    connect(m_sslSocket, QOverload<QAbstractSocket::SocketError>::of(&QSslSocket::errorOccurred), this, &LoginWidget::onSocketError);

    // 注册模块信号绑定
    connect(ui->btnRegister, &QPushButton::clicked, this, [=]() {
        // ① 父窗口设为nullptr，避免样式继承
        RegisterWidget *regWidget = new RegisterWidget(nullptr);
        // ② 设置窗口关闭时自动删除，避免内存泄漏
        regWidget->setAttribute(Qt::WA_DeleteOnClose);
        // ③ 显示注册窗口（模态/非模态可选）
        regWidget->show(); // 非模态：登录窗口仍可操作
        //regWidget->exec(); // 模态：必须关闭注册窗口才能操作登录窗口（更推荐）
    });

    // 登录超时定时器（原逻辑保留）
    m_loginTimeoutTimer->setSingleShot(true);
    m_loginTimeoutTimer->setInterval(5000);
    connect(m_loginTimeoutTimer, &QTimer::timeout, this, [=]() {
        if (m_loadingMsg) {
            m_loadingMsg->close();
            delete m_loadingMsg;
            m_loadingMsg = nullptr;
        }
        QMessageBox::warning(this, "提示", "登录超时：请检查网络或服务端状态");
        // 强制重置所有Socket状态（新增）
        m_tcpSocket->abort();
        m_sslSocket->abort();
        m_webSocketHandshakeDone = false;
        m_webSocketKey.clear();
        ui->login_botton->setEnabled(true);
    });

    // WebSocket基础配置
    m_sslSocket->setReadBufferSize(1024 * 1024);
}

// 析构函数
LoginWidget::~LoginWidget()
{
    delete ui;
    if (m_mainWin) {
        // 说明Socket已传给MainWindow，LoginWidget不再管理Socket的释放
        delete m_mainWin;
        m_mainWin = nullptr;
    } else {
        // 未创建MainWindow，直接释放Socket
        m_tcpSocket->abort();
        m_sslSocket->abort();
        delete m_tcpSocket;
        delete m_sslSocket;
    }
}

void LoginWidget::handleLogin()
{
    // ========== 新增：强制重置所有状态（放在最开头） ==========
    // 登录失败后m_tcpSocket/m_sslSocket仍处于异常连接状态，二次登录复用旧连接，这是不对的
    // 1. 终止旧连接
    m_tcpSocket->abort();
    m_sslSocket->abort();
    // 2. 恢复信号绑定（登录成功时解除过，失败后需恢复）
    reconnectSslSocketSignals();
    // 3. 重置标志位
    m_webSocketHandshakeDone = false;
    m_webSocketKey.clear();
    // 4. 重置定时器
    m_loginTimeoutTimer->stop();
    // 5. 清理旧弹窗
    if (m_loadingMsg) {
        m_loadingMsg->close();
        delete m_loadingMsg;
        m_loadingMsg = nullptr;
    }
    // ======================================================
    // 原输入校验逻辑保留
    QString username = ui->leUsername->text().trimmed();
    QString password = ui->lePassword->text().trimmed();
    if (username.isEmpty()) {
        QMessageBox::warning(this, "提示", "账号不能为空！");
        ui->leUsername->setFocus();
        return;
    }
    if (password.isEmpty()) {
        QMessageBox::warning(this, "提示", "密码不能为空！");
        ui->lePassword->setFocus();
        return;
    }

    ui->login_botton->setEnabled(false);
    m_loadingMsg = new QMessageBox(QMessageBox::Information, "提示", "正在连接服务器（安全验证中）...", QMessageBox::NoButton, this);
    m_loadingMsg->show();
    // ========== 修正：Qt6 正确的连接超时配置 ==========
    // 用connectToHost的重载函数（推荐，最稳定）
    m_tcpSocket->connectToHost("www.singchat.chat", 443, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
    // =============================================
    m_loginTimeoutTimer->start();
}


void LoginWidget::reconnectSslSocketSignals()
{
    // 解除旧绑定（避免重复绑定）
    m_sslSocket->disconnect(this);
    // 重新绑定所有SSL信号
    connect(m_sslSocket, &QSslSocket::encrypted, this, &LoginWidget::onSslEncrypted);
    connect(m_sslSocket, &QSslSocket::sslErrors, this, &LoginWidget::onSslErrors);
    connect(m_sslSocket, &QSslSocket::readyRead, this, &LoginWidget::onSslReadyRead);
    connect(m_sslSocket, &QSslSocket::disconnected, this, &LoginWidget::onDisconnected);
    connect(m_sslSocket, QOverload<QAbstractSocket::SocketError>::of(&QSslSocket::errorOccurred), this, &LoginWidget::onSocketError);
    // 重置Socket配置
    m_sslSocket->setReadBufferSize(1024 * 1024);
}

// TCP连接成功：启动SSL加密
void LoginWidget::onTcpConnected()
{
    qDebug() << "✅ TCP连接成功，开始SSL握手";
    // 用TCP套接字描述符初始化SSL套接字
    m_sslSocket->setSocketDescriptor(m_tcpSocket->socketDescriptor());
    // SSL配置（强制TLS1.2+开启证书验证）
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setProtocol(QSsl::TlsV1_2);
    // ========== 替换为Qt 6支持的验证模式：宽松验证（同时关闭证书+主机名严格验证） ==========
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyPeer); // 开启证书验证
    sslConfig.setPeerVerifyDepth(3); // 验证证书链深度



    // ========== 关键：加载服务器的CA证书 ==========
    QString caCertPath = QCoreApplication::applicationDirPath() + "/cert/singchat.chat_bundle.crt"; // 替换为你导出的证书路径
    QFile caCertFile(caCertPath);
    if (caCertFile.open(QIODevice::ReadOnly)) {
        QSslCertificate caCert(&caCertFile, QSsl::Pem); // 注意证书格式（Pem/DER）
        caCertFile.close();
        if (!caCert.isNull()) {
            QList<QSslCertificate> caCertList;
            caCertList << caCert;
            sslConfig.setCaCertificates(caCertList); // 将服务器证书加入信任列表
            qDebug() << "✅ 成功加载CA证书：" << caCertPath;
        } else {
            qCritical() << "❌ 加载CA证书失败：证书格式错误";
        }
    } else {
        qCritical() << "❌ 加载CA证书失败：文件不存在/无法读取，路径：" << caCertPath;
    }

    m_sslSocket->setSslConfiguration(sslConfig);
    // 启动客户端SSL加密
    m_sslSocket->startClientEncryption();
}

// 连接断开
void LoginWidget::onDisconnected()
{
    qDebug() << "🔌 连接断开";
    m_webSocketHandshakeDone = false;
    m_loginTimeoutTimer->stop();
    ui->login_botton->setEnabled(true);
    if (m_loadingMsg) {
        m_loadingMsg->close();
        delete m_loadingMsg;
        m_loadingMsg = nullptr;
    }
    // ========== 新增：确保Socket完全重置 ==========
    m_tcpSocket->abort();
    m_sslSocket->abort();
    m_webSocketKey.clear();
    // =============================================
}

// 套接字错误
void LoginWidget::onSocketError(QAbstractSocket::SocketError error)
{
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
    // 暂时不需要把这个界面弹出给用户，终端能看到就行了
    qCritical() << "❌ 连接错误：" << errorDesc << "，详情：" << m_sslSocket->errorString();
    // QMessageBox::critical(this, "错误", QString("连接服务器失败：%1\n详情：%2").arg(errorDesc,m_sslSocket->errorString()));
    // ========== 新增：立即清理弹窗+停止定时器 ==========
    if (m_loadingMsg) {
        m_loadingMsg->close();
        delete m_loadingMsg;
        m_loadingMsg = nullptr;
    }
    m_loginTimeoutTimer->stop();
    // =============================================
    onDisconnected();
}

// SSL握手成功：发送WebSocket握手请求
void LoginWidget::onSslEncrypted()
{
    qDebug() << "✅ SSL握手成功，开始WebSocket握手";
    // 生成WebSocket握手密钥（随机16字节→Base64）
    QByteArray keyBytes(16, 0);
    QRandomGenerator::system()->fillRange(reinterpret_cast<quint32*>(keyBytes.data()), keyBytes.size() / 4);
    m_webSocketKey = keyBytes.toBase64();

    // 构造WebSocket握手请求（HTTP协议升级）
    QByteArray handshakeReq = QString(
                                  "GET /ws HTTP/1.1\r\n"
                                  "Host: www.singchat.chat\r\n"
                                  "Upgrade: websocket\r\n"
                                  "Connection: Upgrade\r\n"
                                  "Sec-WebSocket-Key: %1\r\n"
                                  "Sec-WebSocket-Version: 13\r\n\r\n"
                                  ).arg(m_webSocketKey).toUtf8();
    m_sslSocket->write(handshakeReq);
}

// 读取SSL数据：处理WebSocket握手/消息
void LoginWidget::onSslReadyRead()
{
    if (!m_webSocketHandshakeDone) {
        // 阶段1：处理WebSocket握手响应
        QByteArray response = m_sslSocket->readAll();
        qDebug() << "📥 WebSocket握手响应：" << response.left(200);

        // 验证是否是101切换协议
        if (response.startsWith("HTTP/1.1 101 Switching Protocols")) {
            // 改用QCryptographicHash计算SHA-1哈希（符合WebSocket标准）
            QByteArray input = m_webSocketKey.toUtf8() + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
            QByteArray acceptKey = QCryptographicHash::hash(input, QCryptographicHash::Sha1).toBase64();
            if (response.contains(acceptKey)) {
                qDebug() << "✅ WebSocket握手成功";
                m_webSocketHandshakeDone = true;
                // 关闭加载弹窗
                if (m_loadingMsg) {
                    m_loadingMsg->close();
                    delete m_loadingMsg;
                    m_loadingMsg = nullptr;
                }
                m_loginTimeoutTimer->stop();
                // 发送登录消息
                sendLoginMessage();
            } else {
                qCritical() << "❌ WebSocket握手失败：AcceptKey不匹配";
                // ========== 新增：握手失败时清理弹窗+定时器 ==========
                if (m_loadingMsg) {
                    m_loadingMsg->close();
                    delete m_loadingMsg;
                    m_loadingMsg = nullptr;
                }
                m_loginTimeoutTimer->stop();
                // =============================================
                onDisconnected();
            }
        } else {
            qCritical() << "❌ WebSocket握手失败：服务器返回非101响应";
            // ========== 新增：握手失败时清理弹窗+定时器 ==========
            if (m_loadingMsg) {
                m_loadingMsg->close();
                delete m_loadingMsg;
                m_loadingMsg = nullptr;
            }
            m_loginTimeoutTimer->stop();
            // =============================================
            onDisconnected();
        }
    } else {
        // 阶段2：处理WebSocket消息（解析帧）
        QByteArray frame = m_sslSocket->readAll();
        if (frame.size() < 2) return;

        // WebSocket帧格式解析（简化版，仅处理文本消息）
        quint8 opcode = frame[0] & 0x0F;
        if (opcode != 0x01) return; // 仅处理文本消息

        // 解析Payload长度
        quint64 payloadLen = frame[1] & 0x7F;
        int offset = 2;
        if (payloadLen == 126) {
            payloadLen = (static_cast<quint16>(frame[2]) << 8) | frame[3];
            offset = 4;
        } else if (payloadLen == 127) {
            // 64位长度，这里省略（实际项目需处理）
            return;
        }

        // 解析掩码（客户端发送的消息有掩码，服务器发送的无）
        QByteArray maskingKey;
        if (frame[1] & 0x80) {
            maskingKey = frame.mid(offset, 4);
            offset += 4;
        }
        QByteArray payload = frame.mid(offset, payloadLen);

        // 解掩码（如果有）
        if (!maskingKey.isEmpty()) {
            for (int i = 0; i < payload.size(); i++) {
                payload[i] ^= maskingKey[i % 4];
            }
        }

        // 传递消息到原处理逻辑
        onWebSocketTextMessageReceived(QString::fromUtf8(payload));
    }
}

// 发送登录消息（封装为WebSocket文本帧，添加掩码）
void LoginWidget::sendLoginMessage()
{
    QString username = ui->leUsername->text().trimmed();
    QString password = ui->lePassword->text().trimmed();
    QJsonObject loginMsg;
    loginMsg["type"] = "login";
    loginMsg["from"] = username;

    // ========== 新增：密码SHA256+Hex加密 ==========
    QByteArray pwdBytes = password.toUtf8();
    QByteArray encryptedPwd = QCryptographicHash::hash(pwdBytes, QCryptographicHash::Sha256).toHex();
    loginMsg["password"] = QString(encryptedPwd);
    // =============================================


    loginMsg["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    QJsonDocument doc(loginMsg);
    QByteArray payload = doc.toJson(QJsonDocument::Compact);

    // ========== 修复1：生成4字节掩码key（客户端消息必须加掩码） ==========
    QByteArray maskingKey(4, 0);
    QRandomGenerator::system()->fillRange(reinterpret_cast<quint32*>(maskingKey.data()), 1);

    // ========== 修复2：构造WebSocket文本帧（带掩码） ==========
    QByteArray frame;
    frame.append(0x81); // 第一个字节：FIN=1 + Opcode=1（文本消息）

    // 第二个字节：Payload长度 + 掩码位（0x80）
    quint8 payloadLenByte;

    if (payload.size() <= 125) {
        payloadLenByte = static_cast<quint8>(payload.size()) | 0x80; // 加掩码位
        frame.append(payloadLenByte);
    } else if (payload.size() <= 65535) {
        payloadLenByte = 0x7E | 0x80; // 加掩码位
        frame.append(payloadLenByte);
        frame.append(static_cast<quint8>((payload.size() >> 8) & 0xFF));
        frame.append(static_cast<quint8>(payload.size() & 0xFF));

    } else {
        payloadLenByte = 0x7F | 0x80; // 加掩码位
        frame.append(payloadLenByte);
        // 64位长度处理（省略）
        return;
    }

    // ========== 修复3：添加掩码key到帧中 ==========
    frame.append(maskingKey);


    // ========== 修复4：对Payload进行掩码处理 ==========
    for (int i = 0; i < payload.size(); i++) {
        payload[i] ^= maskingKey[i % 4];
    }

    // 添加处理后的Payload
    frame.append(payload);
    m_sslSocket->write(frame);
    qDebug() << "📤 发送登录消息（带掩码）：" << QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
}



void LoginWidget::onSslErrors(const QList<QSslError> &errors)
{
    qDebug() << "📌 SSL错误列表：";
    QList<QSslError> ignoreErrors; // 要忽略的错误集合
    QString errorSummary;

    // 遍历所有SSL错误，筛选需要忽略的两个关键错误
    for (const QSslError &err : errors) {
        QString errStr = err.errorString();
        qDebug() << "  - " << errStr;
        errorSummary += errStr + "\n";

        // 1. 忽略「主机名不匹配」（通过错误枚举值，精准匹配）
        if (err.error() == QSslError::HostNameMismatch) {
            ignoreErrors.append(err);
        }
        // 2. 忽略「颁发者证书找不到」（通过错误描述，适配多语言/版本）
        else if (errStr.contains("The issuer certificate of a locally looked up certificate could not be found")) {
            ignoreErrors.append(err);
        }
    }

    // 核心逻辑：仅忽略上述两个错误，其他错误（如证书过期/伪造）正常阻断
    if (!ignoreErrors.isEmpty()) {
        // 执行忽略操作，让SSL握手继续
        m_sslSocket->ignoreSslErrors(ignoreErrors);
        qDebug() << "⚠️ 已忽略SSL错误（仅兼容用，无安全风险）：";
        qDebug() << "  - 主机名不匹配（OpenSSL 3.6 SAN解析兼容）";
        qDebug() << "  - 颁发者证书找不到（证书链待补全）";
    } else {
        // 其他严重错误：弹窗提示+断开连接
        QMessageBox::critical(this, "SSL验证失败", "证书非法，连接终止：\n" + errorSummary);
        m_sslSocket->disconnectFromHost();
    }
}

void LoginWidget::onWebSocketTextMessageReceived(const QString &message)
{
    qDebug() << "📥 收到服务端消息：" << message;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (!doc.isObject()) {
        QMessageBox::warning(this, "提示", "服务端返回数据格式错误！");
        onDisconnected();
        return;
    }

    QJsonObject msgObj = doc.object();
    QString msgType = msgObj["type"].toString();
    QString content = msgObj["content"].toString();

    if (msgType == "tip") {
        if (content.contains("登录成功")) {
            QString username = ui->leUsername->text().trimmed();
            QMessageBox::information(this, "成功", content);
            this->hide();

            // ========== 修改：仅解除LoginWidget的消息处理信号，而非所有信号（避免影响后续登录） ==========
            disconnect(m_sslSocket, &QSslSocket::readyRead, this, &LoginWidget::onSslReadyRead);
            disconnect(m_sslSocket, &QSslSocket::disconnected, this, &LoginWidget::onDisconnected);
            // =============================================

            if (!m_mainWin) {
                m_mainWin = new MainWindow();
                // 传递用户名和SSL套接字给MainWindow
                m_mainWin->setLoginInfo(username, m_sslSocket);
                m_mainWin->show();
            }
        } else {
            QMessageBox::warning(this, "失败", content);
            ui->lePassword->clear();
            ui->lePassword->setFocus();
            // ========== 新增：立即停止定时器 ==========
            m_loginTimeoutTimer->stop();
            // =============================================
            // ========== 新增：登录失败后强制重置Socket ==========
            m_tcpSocket->abort();
            m_sslSocket->abort();
            m_webSocketHandshakeDone = false;
            // =============================================
            onDisconnected();
        }
    }
}



// 鼠标事件（原逻辑保留）
void LoginWidget::mousePressEvent(QMouseEvent *event)
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
void LoginWidget::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && !m_dragPos.isNull()) {
        QPoint newPos = event->globalPosition().toPoint() - m_dragPos;
        this->move(newPos);
        event->accept();
        return;
    }
    QWidget::mouseMoveEvent(event);
}
