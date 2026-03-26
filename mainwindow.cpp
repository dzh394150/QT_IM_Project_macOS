#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "FriendListModel.h"
#include "FriendRequestModel.h"
#include "AddFriends.h"
#include <QRegularExpression> // 【0112新增】替代QRegExp
#include <QMessageBox>
#include <QRandomGenerator>
#include <QCryptographicHash>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QBuffer>
#include <QImage>
#include <QSslCipher>
#include <QMouseEvent>
#include "FriendRequestDelegate.h"
#include "FriendListDelegate.h"
#include <QScrollBar>
#include <QStyleFactory>
#include <QButtonGroup>
#include <QTextBlock>
#include <QImage>
#include <QElapsedTimer>
#include <QTextCursor>
#include <QTextImageFormat>
#include <QStyle>
#include <QCommonStyle> // Qt 6 必备，支持扩展标准图标枚举
#include <QIcon>
#include <qmath.h>
#include <QTimer>
#include <QCoreApplication> // 获取程序运行路径
#include <QDir>             // 路径拼接、目录判断
// 【0118晚上群聊功能实现开始】
#include "CreateGroup.h"  // 引入创建群聊窗口的头文件
// 【0118晚上群聊功能实现结束】
#include <FileUtils.h>
#include "LocalChatCache.h"

// ---------------------- 0110文件传输：初始化MainWindow类的静态文件格式常量 ----------------------
const QStringList MainWindow::SUPPORT_IMAGE_FORMATS = {".png", ".jpg", ".jpeg", ".gif"};
const QStringList MainWindow::SUPPORT_VIDEO_FORMATS = {".mp4", ".mov", ".avi"};
const QStringList MainWindow::SUPPORT_COMPRESS_FORMATS = {".zip", ".rar", ".7z"};
// 【0110晚上新增】初始化分片大小常量
// 静态常量 MainWindow::FILE_CHUNK_SIZE 的类外初始化代码不能放在头文件（.h）中
// 否则导致多个源文件（.cpp）包含该头文件时，生成了多份该常量的定义，链接阶段冲突。
const qint64 MainWindow::FILE_CHUNK_SIZE = 256 * 1024; // 256KB的分片大小

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_sslSocket(nullptr)
    , m_netManager(nullptr)
    , m_currentUser("")
    , m_selectedFriend("")
    , m_friendListModel(new FriendListModel(this))
    , m_friendRequestModel(new FriendRequestModel(this))
    , m_selectedRequestFromUser("")
    , m_chatModel(new ChatModel(this)) // 关键：初始化chatModel，指针没有初始化直接被操作系统闪退，还捕捉不到报错
    , m_addFriendsDialog(nullptr)
    , m_chunkSendWaitLock(false)  // 【0110晚上新增】初始化分片等待锁
    , m_lastChunkSendSuccess(false) // 【0110晚上新增】初始化分片发送状态
    , m_currentTransferFileId("")   // 【0110晚上新增】初始化当前传输文件ID

    // 【0117晚上文件异步发送修改开始】
    , m_sendFile(nullptr)
    , m_fileSendTotalSize(0)
    , m_fileSendHasTransferred(0)
    , m_sendTotalChunks(0)
    , m_sendCurrentChunkIndex(0)
    , m_sendFileType("")
    // 【事件驱动优化】初始化新增成员变量
    , m_currentChunkSize(0)
    , m_chunkTimeoutTimer(nullptr)
    // 【0117晚上文件异步发送修改结束】

    , m_imagePreviewDialog(new ImagePreviewDialog(this))  // 【0113晚上新增功能：初始化大图预览弹窗】
    , m_fileDownloader(new FileDownloader(this))            // 【0113晚上新增功能：初始化文件下载工具类】
    , m_createGroupDialog(nullptr)
    , m_friendDelegate(nullptr)

    // 【0201白天QT心跳包逻辑开始：初始化重连TCP Socket】
    , m_heartbeatSendTimer(nullptr)
    , m_pongTimeoutTimer(nullptr)
    , m_isWebSocketConnected(false)
{
    // 强制使用Qt Fusion样式（彻底避开系统原生滚动条）——解决滚动条样式表不生效的问题
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    ui->setupUi(this);
    // 【0117文件传输相关：初始隐藏进度条】
    ui->fileprogress->hide();

    // 【0113晚上新增功能：绑定聊天视图点击信号到自定义槽函数】
    connect(ui->chat_info_show, &QListView::clicked,
            this, &MainWindow::onChatViewClicked);
    // 【0112新增】初始化全局缓存，确保为空
    m_frameCache.clear();

    // 【0110新增】关键：延后缓存chat_input的原始样式表（此时ui样式已加载）
    ui->chat_input->cacheOriginalStyleSheet();

    // ===================== 核心互斥代码开始 =====================
    // 1. 创建QButtonGroup对象，父对象设为this（利用Qt父子机制自动管理内存，避免内存泄漏）
    QButtonGroup *exclusiveBtnGroup = new QButtonGroup(this);

    // 2. 向按钮组中添加需要互斥的两个按钮：btnStartChat 和 btnLookFriend
    exclusiveBtnGroup->addButton(ui->btnStartChat);
    exclusiveBtnGroup->addButton(ui->btnLookFriend);
    exclusiveBtnGroup->addButton(ui->groupchat);
    // 3. 开启按钮组互斥模式（核心：实现同一组仅一个按钮可处于选中状态，且再次点击自身不取消选中）
    exclusiveBtnGroup->setExclusive(true);
    // ===================== 核心互斥代码结束 =====================



    // ========== 初始化好友请求红点标签 ==========
    m_notificationBadge = new QLabel(ui->btnLookFriend); // 父对象设为按钮，跟随按钮移动
    m_notificationBadge->setObjectName("notificationBadge");
    m_notificationBadge->hide(); // 默认隐藏
    // 红点样式：圆形、红色背景、白色数字
    m_notificationBadge->setStyleSheet(R"(
        QLabel#notificationBadge {
            background-color: #FF3B30; /* 微信红 */
            color: white;
            font-size: 12px;
            font-weight: bold;
            border-radius: 9px; /* 圆形 */
            min-width: 18px;    /* 最小宽度 */
            min-height: 18px;   /* 最小高度 */
            max-width: 18px;
            max-height: 18px;
            text-align: center; /* 文字水平居中 */
        }
    )");
    // 设置红点对齐方式：按钮右下角
    m_notificationBadge->setAlignment(Qt::AlignCenter);
    m_notificationBadge->setAttribute(Qt::WA_TransparentForMouseEvents); // 穿透鼠标事件（不影响按钮点击）

    // ==================以上是好友请求红点逻辑====================

    // ========== 统一配置所有QListView的丝滑滚动（核心） ==========
    // 1. chat_info_show（原有配置，保留）
    ui->chat_info_show->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->chat_info_show->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->chat_info_show->verticalScrollBar()->setPageStep(30);
    ui->chat_info_show->verticalScrollBar()->setSingleStep(5);

    // 2. person_list（新增：和chat_info_show完全一致的配置）
    ui->person_list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->person_list->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->person_list->verticalScrollBar()->setPageStep(30);
    ui->person_list->verticalScrollBar()->setSingleStep(5);

    // 3. friendRequest_list（新增：和chat_info_show完全一致的配置）
    ui->friendRequest_list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->friendRequest_list->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->friendRequest_list->verticalScrollBar()->setPageStep(30);
    ui->friendRequest_list->verticalScrollBar()->setSingleStep(5);

    // ========== 初始化所有列表的滚动条样式（默认隐藏） ==========
    showListViewScrollBar(ui->chat_info_show, false);    // 原有
    showListViewScrollBar(ui->person_list, false);       // 新增
    showListViewScrollBar(ui->friendRequest_list, false); // 新增


    // ========== 点击按钮切换页面后，隐藏相关控件 ==========
    ui->btnAgree->hide();
    ui->btnRefuse->hide();
    ui->cheakinfoArea->hide();
    ui->nameArea->hide();
    ui->photoArea->hide();
    ui->Friend_status->hide();
    // =====================================================
    // 设置好友请求列表的委托
    ui->friendRequest_list->setItemDelegate(new FriendRequestDelegate(ui->friendRequest_list));
    ui->friendRequest_list->setModel(m_friendRequestModel);

    // 设置好友列表的委托
    m_friendDelegate = new FriendListDelegate(this);
    ui->person_list->setItemDelegate(m_friendDelegate);
    ui->person_list->setModel(m_friendListModel);
    // ==============0106新增设置聊天列表的模型和委托
    ui->chat_info_show->setModel(m_chatModel);
    auto *chatDelegate = new ChatDelegate(ui->chat_info_show);
    chatDelegate->setSslConfiguration(m_sslConf);
    ui->chat_info_show->setItemDelegate(chatDelegate);
    // 连接滚动条信号用于懒加载
    connect(ui->chat_info_show->verticalScrollBar(), &QScrollBar::valueChanged, this, &MainWindow::onChatViewScrollValueChanged);
    
    // 注意：移除了 Batched 布局模式，因为它与样式表动态切换冲突，会导致滚动条跳动
    // 批处理布局在设置样式表时会触发延迟布局计算，导致滚动位置不稳定

    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    isDragging = false;
    isResizing = false;
    resizeEdge = 0;

    // 安装事件过滤器
    qApp->installEventFilter(this);
    // ===== 0105新增：给输入框安装事件过滤器 =====
    ui->chat_input->installEventFilter(this);
    // 原有连接
    connect(ui->person_list, &QListView::clicked,
            this, &MainWindow::onPersonListItemClicked);
    connect(ui->friendRequest_list, &QListView::clicked,
            this, &MainWindow::onFriendRequestListItemClicked);
    connect(ui->send_botton, &QPushButton::clicked,
            this, &MainWindow::onSendButtonClicked);
    connect(ui->btnLookFriend, &QPushButton::clicked,
            this, &MainWindow::onBtnLookFriendClicked);
    connect(ui->btnAgree, &QPushButton::clicked,
            this, &MainWindow::onBtnAgreeClicked);
    connect(ui->btnRefuse, &QPushButton::clicked,
            this, &MainWindow::onBtnRefuseClicked);
    // 【0120白天绑定群聊list点击事件】
    connect(ui->group_list, &QListView::clicked,
            this, &MainWindow::onGroupListItemClicked);

    // 新增连接
    connect(ui->btnAddFriends, &QPushButton::clicked,
            this, &MainWindow::onBtnAddFriendsClicked);
    connect(ui->btnStartChat, &QPushButton::clicked,
            this, &MainWindow::onBtnStartChatClicked);
    // 【0118群聊绑定信号】
    connect(ui->groupchat, &QPushButton::clicked,
            this, &MainWindow::ongroupchatClicked);


    //关闭窗口
    connect(ui->close_botton, &QPushButton::clicked,
            this, &AddFriends::close);
    connect(ui->close_botton_2, &QPushButton::clicked,
            this, &AddFriends::close);
    // 最小化按钮
    connect(ui->minimize_botton, &QPushButton::clicked,
            this, &MainWindow::showMinimized);
    connect(ui->minimize_botton_2, &QPushButton::clicked,
            this, &MainWindow::showMinimized);
    // 最大化按钮（切换最大化/还原）
    connect(ui->maximize_botton, &QPushButton::clicked,
            this, &MainWindow::toggleMaximize);
    connect(ui->maximize_botton_2, &QPushButton::clicked,
            this, &MainWindow::toggleMaximize);

    // 【0117新增：绑定下载进度信号到 MainWindow 自定义槽函数】
    connect(m_fileDownloader, &FileDownloader::downloadProgress,
            this, &MainWindow::onFileDownloadProgressUpdated);
    // 【0117新增：绑定下载完成信号（现在就能触发你的onFileDownloadFinished槽函数）】
    connect(m_fileDownloader, &FileDownloader::downloadFinished,
            this, &MainWindow::onFileDownloadFinished);

    // 【0117新增：绑定下载失败信号（触发你的onFileDownloadFailed槽函数）】
    connect(m_fileDownloader, &FileDownloader::downloadFailed,
            this, &MainWindow::onFileDownloadFailed);
    // 【0117晚上文件异步发送修改开始】
    // 【0324修复：删除无效的信号槽连接 - onAsyncSendNextChunk是槽函数不是信号】
    // 【0117晚上文件异步发送修改结束】
    
    // 【0320进阶优化】初始化文件传输工作线程
    m_fileTransferThread = new QThread(this);
    m_fileTransferWorker = new FileTransferWorker();
    m_fileTransferWorker->moveToThread(m_fileTransferThread);
    m_isLoading = false; // 初始化懒加载状态标记
    
    // 连接工作线程的信号槽
    connect(this, &MainWindow::startProcessChunk, m_fileTransferWorker, &FileTransferWorker::processFileChunk);
    connect(m_fileTransferWorker, &FileTransferWorker::chunkProcessed, this, &MainWindow::onChunkProcessed);
    connect(m_fileTransferWorker, &FileTransferWorker::processError, this, &MainWindow::onProcessError);
    
    // 启动工作线程
    m_fileTransferThread->start();
    ui->friendRequest_list->setModel(m_friendRequestModel);

    // ======================0110新增文件传输逻辑================
    // 【新增】连接ChatInputTextEdit的文件拖拽信号与MainWindow的槽函数
    // ctrl+V粘贴文件后，最终也会触发这个信号，不必针对ctrl+V额外做新增
    connect(ui->chat_input, &ChatInputTextEdit::fileDropped, this, &MainWindow::onFileDropped);
    // 【新增】绑定聊天输入框内容变更信号，监控预览图是否被删除
    connect(ui->chat_input, &QTextEdit::textChanged, this, &MainWindow::on_chatInput_textChanged);
    // 【0110晚上新增】初始化分片传输相关变量（补充到构造函数末尾）
    m_chunkSendWaitLock = false;
    m_lastChunkSendSuccess = false;
    m_currentTransferFileId = "";

    // 【0120白天群聊列表渲染相关逻辑开始：初始化群聊列表模型和委托，绑定到group_list控件】
    m_groupListModel = new GroupListModel(this);
    m_groupListDelegate = new GroupListDelegate(ui->group_list);
    ui->group_list->setModel(m_groupListModel);
    ui->group_list->setItemDelegate(m_groupListDelegate);
    // 【0120白天群聊列表渲染相关逻辑结束】


    // 【0123未读消息逻辑开始：初始化私聊/群聊红点控件】
    // 1. 初始化私聊按钮红点（绑定到btnStartChat）
    m_chatBadge = new QLabel(ui->btnStartChat);
    m_chatBadge->setStyleSheet(R"(
        QLabel {
            background-color: #FF3B30; /* 微信红 */
            color: white;
            font-size: 12px;
            font-weight: bold;
            border-radius: 9px; /* 圆形（直径18px，半径9px） */
            text-align: center;
        }
    )");
    m_chatBadge->setFixedSize(18, 18); // 固定尺寸（和好友申请红点一致）
    m_chatBadge->hide(); // 默认隐藏

    // 2. 初始化群聊按钮红点（绑定到groupchat）
    m_groupChatBadge = new QLabel(ui->groupchat);
    m_groupChatBadge->setStyleSheet(m_chatBadge->styleSheet()); // 复用样式
    m_groupChatBadge->setFixedSize(18, 18);
    m_groupChatBadge->hide(); // 默认隐藏
    // 【0123未读消息逻辑结束】


}

MainWindow::~MainWindow()
{
    // 【0320进阶优化】清理工作线程资源
    if (m_fileTransferThread) {
        m_fileTransferThread->quit();
        m_fileTransferThread->wait();
        delete m_fileTransferWorker;
        delete m_fileTransferThread;
    }
    
    // 【0201白天QT心跳包逻辑开始：清理心跳/重连资源】
    if (m_heartbeatSendTimer) {
        m_heartbeatSendTimer->stop();
        delete m_heartbeatSendTimer;
        m_heartbeatSendTimer = nullptr;
    }
    if (m_pongTimeoutTimer) {
        m_pongTimeoutTimer->stop();
        delete m_pongTimeoutTimer;
        m_pongTimeoutTimer = nullptr;
    }
    if (m_sslSocket) {
        m_sslSocket->abort();
        delete m_sslSocket;
    }
    delete ui;
}

void MainWindow::setLoginInfo(const QString &username, QSslSocket *sslSocket)
{
    m_currentUser = username;
    m_sslSocket = sslSocket;
    m_sslSocket->setParent(this);
    m_sslConf = sslSocket->sslConfiguration();
    
    // 初始化本地聊天缓存
    if (!LocalChatCache::instance()->init(username)) {
        qDebug() << "⚠️ Failed to initialize local chat cache";
    } else {
        qDebug() << "✅ Local chat cache initialized for user:" << username;
    }
    // 【0201白天QT心跳包逻辑开始：初始化心跳/重连定时器】
    // 1. 初始化心跳发送定时器（30秒发送一次ping）
    m_heartbeatSendTimer = new QTimer(this); // 父对象设为this，自动管理内存
    m_heartbeatSendTimer->setInterval(30000); // 30秒/次（可根据服务端调整）
    connect(m_heartbeatSendTimer, &QTimer::timeout, this, &MainWindow::sendPingMessage);

    // 2. 初始化pong超时定时器（60秒超时）
    m_pongTimeoutTimer = new QTimer(this);
    m_pongTimeoutTimer->setSingleShot(true); // 单次触发（收到pong后重置）
    m_pongTimeoutTimer->setInterval(60000); // 60秒超时
    connect(m_pongTimeoutTimer, &QTimer::timeout, this, &MainWindow::onPongTimeout);

    m_netManager = new QNetworkAccessManager(this);
    // 绑定错误，断开连接，收到消息三个信号
    connect(m_netManager, &QNetworkAccessManager::sslErrors,
            this, &MainWindow::onSslErrors);

    connect(m_sslSocket, &QSslSocket::readyRead, this, &MainWindow::onSslReadyRead);

    connect(m_sslSocket, &QSslSocket::disconnected, this, &MainWindow::onWebSocketDisconnected);
    // 【0201白天QT心跳包逻辑开始：绑定Socket错误信号】
    connect(m_sslSocket, QOverload<QAbstractSocket::SocketError>::of(&QSslSocket::errorOccurred), this, &MainWindow::onSocketError);
    // 【0201白天QT心跳包逻辑结束】
    // 先从本地缓存加载好友列表
    QList<FriendData> cachedFriends = LocalChatCache::instance()->getAllUserInfos();
    if (!cachedFriends.isEmpty()) {
        qDebug() << "🔹 [MainWindow::setLoginInfo] 从本地缓存加载好友列表，共" << cachedFriends.size() << "个好友";
        for (const auto &friendData : cachedFriends) {
            m_friendAvatars[friendData.username] = friendData.avatarUrl; // 更新内存缓存
        }
        loadFriendList(cachedFriends);
    }

    // 先从本地缓存加载群聊列表
    QList<GroupInfo> cachedGroups = LocalChatCache::instance()->getAllGroupInfos();
    if (!cachedGroups.isEmpty()) {
        qDebug() << "🔹 [MainWindow::setLoginInfo] 从本地缓存加载群聊列表，共" << cachedGroups.size() << "个群聊";
        m_groupListModel->clearGroups();
        for (const auto &groupInfo : cachedGroups) {
            m_groupListModel->addGroup(groupInfo);
        }
    }

    // 登录后立即请求当前用户信息（获取数据库里的头像URL）
    QJsonObject reqMsg;
    reqMsg["type"] = "get_user_info";
    reqMsg["username"] = m_currentUser;
    qDebug() << "🔹 [MainWindow::setLoginInfo] 发送 get_user_info 请求：" << reqMsg;
    sendWebSocketMessage(QJsonDocument(reqMsg).toJson(QJsonDocument::Compact));

    // 登录后立即请求好友列表（更新最新数据）
    QJsonObject reqMsg2;
    reqMsg2["type"] = "get_friends";
    reqMsg2["from"] = m_currentUser;
    reqMsg2["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    qDebug() << "🔹 [MainWindow::setLoginInfo] 发送 get_friends 请求：" << reqMsg2;
    sendWebSocketMessage(QJsonDocument(reqMsg2).toJson(QJsonDocument::Compact));

    // 加载好友请求并显示红点
    qDebug() << "🔹 [MainWindow::setLoginInfo] 调用 loadFriendRequests";
    loadFriendRequests();
    // 【0201白天QT心跳包逻辑开始：初始化心跳/重连定时器】
    // 1. 初始化心跳发送定时器（30秒发送一次ping）
    m_heartbeatSendTimer = new QTimer(this); // 父对象设为this，自动管理内存
    m_heartbeatSendTimer->setInterval(30000); // 30秒/次（可根据服务端调整）
    connect(m_heartbeatSendTimer, &QTimer::timeout, this, &MainWindow::sendPingMessage);

    // 2. 初始化pong超时定时器（60秒超时）
    m_pongTimeoutTimer = new QTimer(this);
    m_pongTimeoutTimer->setSingleShot(true); // 单次触发（收到pong后重置）
    m_pongTimeoutTimer->setInterval(60000); // 60秒超时
    connect(m_pongTimeoutTimer, &QTimer::timeout, this, &MainWindow::onPongTimeout);
    // 4. 初始化状态变量
    m_isWebSocketConnected = true;

    // 5. 启动心跳发送定时器（登录成功立即开始心跳）
    m_heartbeatSendTimer->start();
    qDebug() << "✅ 心跳定时器初始化完成，30秒发送一次ping，pong超时阈值60秒";
    // 【0201白天QT心跳包逻辑结束】
}

// 【0112晚上修正】大于64KB的大文件不能直接丢弃，补充发送大文件的逻辑
void MainWindow::sendWebSocketMessage(const QString &message)
{
    QByteArray payload = message.toUtf8();
    QByteArray maskingKey(4, 0);
    QRandomGenerator::system()->fillRange(reinterpret_cast<quint32*>(maskingKey.data()), 1);

    QByteArray frame;
    // 第一步：写入帧类型（0x81 表示文本帧，FIN 位为 1，表示完整帧）
    frame.append(0x81);

    // 第二步：写入长度字段（补全 0x7F 分支逻辑，支持超大 payload）
    quint64 payloadSize = payload.size();
    if (payloadSize <= 125) {
        // 格式1：1 字节长度（带掩码位 0x80）
        frame.append(static_cast<quint8>(payloadSize) | 0x80);
    } else if (payloadSize <= 65535) {
        // 格式2：3 字节长度（0x7E + 2 字节大端序长度，带掩码位 0x80）
        frame.append(static_cast<quint8>(0x7E | 0x80));
        frame.append(static_cast<quint8>((payloadSize >> 8) & 0xFF)); // 高8位
        frame.append(static_cast<quint8>(payloadSize & 0xFF)); // 低8位
    } else {
        // 格式3：9 字节长度（0x7F + 8 字节大端序长度，带掩码位 0x80）
        frame.append(static_cast<quint8>(0x7F | 0x80));
        // 写入 8 字节大端序长度（从高到低，依次写入每 1 字节）
        for (int i = 7; i >= 0; --i) {
            frame.append(static_cast<quint8>((payloadSize >> (8 * i)) & 0xFF));
        }
        // 移除原有错误的 return 语句
    }

    // 第三步：写入掩码键 + 对 payload 进行掩码加密（原有逻辑，保留不变）
    frame.append(maskingKey);
    for (int i = 0; i < payload.size(); ++i) {
        payload[i] ^= maskingKey[i % 4];
    }
    frame.append(payload);

    // 第四步：发送完整帧（原有逻辑，保留不变）
    m_sslSocket->write(frame);
}

void MainWindow::onBtnLookFriendClicked()
{
    qDebug() << "🔘 [onBtnLookFriendClicked] 点击查看好友请求按钮";

    ui->stackedWidget3->setCurrentWidget(ui->NewFriendInfoWidget);
    ui->stackedWidget2->setCurrentWidget(ui->stackedWidget2_info);

    // ========== 新增：点击按钮切换页面后，隐藏相关控件 ==========
    ui->btnAgree->hide();
    ui->btnRefuse->hide();
    ui->cheakinfoArea->hide();
    ui->nameArea->hide();
    ui->photoArea->hide();
    ui->Friend_status->hide();
    // =====================================================

    qDebug() << "📄 [onBtnLookFriendClicked] 当前页面：" << ui->stackedWidget3->currentWidget()->objectName();
    qDebug() << "🔗 [onBtnLookFriendClicked] 列表绑定的模型：" << ui->friendRequest_list->model();
    qDebug() << "🔗 [onBtnLookFriendClicked] 好友请求模型：" << m_friendRequestModel;

    loadFriendRequests();
}


void MainWindow::loadFriendRequests()
{
    QString urlStr = "https://www.singchat.chat/friend/requests?username=" + m_currentUser;
    qDebug() << "🔍 发起好友请求列表请求：" << urlStr;

    QNetworkRequest req{QUrl(urlStr)};
    req.setSslConfiguration(m_sslConf);

    QNetworkReply *reply = m_netManager->get(req);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        qDebug() << "✅ 好友请求列表请求完成";

        // 打印原始返回数据
        QByteArray data = reply->readAll();
        qDebug() << "📥 好友列表请求原始返回数据：" << data;

        // 解析 JSON
        QJsonParseError jsonErr;
        QJsonDocument doc = QJsonDocument::fromJson(data, &jsonErr);
        if (jsonErr.error != QJsonParseError::NoError) {
            qCritical() << "❌ 好友列表请求 JSON解析失败：" << jsonErr.errorString();
            reply->deleteLater();
            return;
        }

        // 检查是否是数组
        if (!doc.isArray()) {
            qCritical() << "❌ 好友列表请求 返回数据不是JSON数组：" << doc;
            reply->deleteLater();
            return;
        }

        QJsonArray arr = doc.array();
        qDebug() << "📊  好友请求数组大小：" << arr.size();

        // 清空旧数据
        m_friendRequestModel->clearRequests();

        // 新增：统计未处理（status=0）的好友请求数量
        int unreadCount = 0;

        // 解析每个好友请求
        for (auto it = arr.constBegin(); it != arr.constEnd(); ++it) {
            const QJsonValue &val = *it;
            if (!val.isObject()) {
                qWarning() << "⚠️ 好友列表 数组元素不是对象：" << val;
                continue;
            }

            QJsonObject obj = val.toObject();
            FriendRequestData reqData;
            reqData.fromUser = obj["FromUser"].toString();
            reqData.avatarUrl = obj["FromAvatarURL"].toString();
            reqData.message = obj["Message"].toString();
            reqData.status = obj["Status"].toInt(); // 解析状态

            // 统计未处理请求
            if (reqData.status == 0) {
                unreadCount++;
            }

            //qDebug() << "👤  解析到好友请求："
                     // << "fromUser=" << reqData.fromUser
                     // << "avatarUrl=" << reqData.avatarUrl
                     // << "message=" << reqData.message
                     // << "status=" << reqData.status;

            m_friendRequestModel->addRequest(reqData);
        }

        // 核心修改：传递未处理数量给红点显示函数
        qDebug() << "🔴  未处理好友请求数：" << unreadCount;
        showFriendRequestRedDot(unreadCount);

        reply->deleteLater();
    });

    // 错误处理
    connect(reply, &QNetworkReply::errorOccurred, this, [=](QNetworkReply::NetworkError code) {
        qCritical() << "❌  请求错误，错误内容：" << code << reply->errorString();
        // 【0116晚上修正：忽略错误还是加上吧，不然总是时不时有问题】
        // 测试场景：忽略SSL错误，强制继续请求（让握手失败的请求恢复执行）
        reply->ignoreSslErrors();
        reply->deleteLater();

    });
}

void MainWindow::showFriendRequestRedDot(int unreadCount)
{
    QMetaObject::invokeMethod(this, [=]() {
        if (unreadCount > 0) {
            // 1. 设置红点数字（超过9显示9+）
            QString displayText = (unreadCount > 9) ? "9+" : QString::number(unreadCount);
            m_notificationBadge->setText(displayText);

            // 2. 定位红点到按钮右下角（关键：计算坐标）
            QRect btnRect = ui->btnLookFriend->rect();
            int badgeWidth = m_notificationBadge->width();
            int badgeHeight = m_notificationBadge->height();
            // 红点位置：右边缘-2px，下边缘-2px
            int x = btnRect.width() - badgeWidth - 2;
            int y = btnRect.height() - badgeHeight - 2;
            m_notificationBadge->setGeometry(x, y, badgeWidth, badgeHeight);

            // 3. 显示红点
            m_notificationBadge->show();
        } else {
            // 无未处理请求，隐藏红点
            m_notificationBadge->hide();
        }

        qDebug() << "✅ 红点样式已更新，未处理数量：" << unreadCount;
    }, Qt::QueuedConnection);
}

// 【0112下午修正】
// 移除了你原有代码中 if (framePayload.isEmpty()) { continue; }，该逻辑会过滤掉分片结束帧（空 payload）。
// 替换为兼容分片场景的空值判断，仅对「非分片、非延续帧」的空 payload 跳过，避免丢失分片结束标记。
void MainWindow::onSslReadyRead()
{
    // 步骤1：读取当前TCP缓冲区所有新数据，追加到全局缓存（解决TCP分批问题）
    QByteArray newData = m_sslSocket->readAll();
    if (newData.isEmpty()) {
        qDebug() << "===== 无新数据，跳过 =====";
        return;
    }
    // 2. 新增：转发数据给CreateGroup窗口（如果窗口已创建且显示）
    if (m_createGroupDialog && m_createGroupDialog->isVisible()) {
        qDebug() << "【主窗口】将数据转发给CreateGroup窗口";
        emit signalGroupSocketDataArrived(newData); // 发送自定义信号，传递数据
    }
    // 追加到全局帧缓存，拼接分批到达的TCP数据
    m_frameCache.append(newData);
    qDebug() << "===== 缓存拼接后总长度：" << m_frameCache.size() << "字节 =====";

    // 【0112下午新增】：分片缓存前置初始化（防止野值，确保非分片状态下缓存清空）
    if (!m_isReceivingFragmentedMessage) {
        m_messageFragmentCache.clear();
    }

    // 步骤2：循环提取缓存中的完整WebSocket帧（解决WebSocket分片问题）
    while (true) {
        // 步骤2.1：计算缓存中第一个完整WebSocket帧的长度
        quint64 completeFrameLen = calculateCompleteWebSocketFrameLength(m_frameCache);
        if (completeFrameLen == 0 || static_cast<quint64>(m_frameCache.size()) < completeFrameLen) {
            qDebug() << "===== 缓存数据不足，等待后续数据 =====";
            break; // 缓存不足，跳出循环等待下一次数据到达
        }

        // 步骤2.2：从缓存中提取完整的WebSocket帧
        QByteArray completeFrame = m_frameCache.left(static_cast<int>(completeFrameLen));
        m_frameCache = m_frameCache.mid(static_cast<int>(completeFrameLen)); // 移除已解析的帧
        qDebug() << "===== 提取到完整WebSocket帧，剩余缓存长度：" << m_frameCache.size() << "字节 =====";

        // 步骤2.3：解析完整帧，获取分片信息和payload（保留所有帧解析逻辑）
        bool isFinalFrame = false;
        quint8 opcode = 0x00; // 【0112下午新增】：接收解析出的 opcode（统一从parseWebSocketFrame获取）
        QByteArray framePayload;
        // 【0112下午新增】：修改函数调用，传递opcode输出参数（不再手动解析opcode，避免冗余）
        QString frameMessage = parseWebSocketFrame(completeFrame, isFinalFrame, opcode, framePayload);

        // 【0112下午新增】：删除错误的「直接跳过空payload」逻辑，替换为分片场景兼容判断
        // 原有错误逻辑：if (framePayload.isEmpty()) { continue; }
        // 新逻辑：仅对「非分片帧且非延续帧」判断空payload，避免过滤分片结束帧
        if (framePayload.isEmpty() && !m_isReceivingFragmentedMessage && opcode != 0x00) {
            qDebug() << "===== 非分片帧 payload 为空，跳过 =====";
            continue;
        }

        // 步骤2.4：分片拼接（核心：拼接完整消息，支持大文件相关消息）
        // 【0112下午新增】：删除重复的opcode解析，直接使用parseWebSocketFrame返回的opcode（统一且准确）
        // 移除：quint8 firstByte = static_cast<quint8>(completeFrame.at(0)); opcode = firstByte & 0x0F;

        // 初始化/拼接分片消息（优化逻辑，兼容FIN位标记，解决空结束帧问题）
        if (opcode == 0x01) { // 文本帧：开始新消息（无论是否为最后一帧）
            m_isReceivingFragmentedMessage = true;
            m_messageFragmentCache.clear();
            m_messageFragmentCache.append(framePayload);
            qDebug() << "===== 【分片】初始化新消息缓存，当前缓存长度：" << m_messageFragmentCache.size() << "字节 =====";
        } else if (opcode == 0x00 && m_isReceivingFragmentedMessage) { // 延续帧：拼接消息（兼容空payload）
            m_messageFragmentCache.append(framePayload);
            qDebug() << "===== 【分片】拼接延续帧，当前缓存总长度：" << m_messageFragmentCache.size() << "字节 =====";
        }

        // 步骤2.5：分片接收完成，执行所有业务逻辑（核心：保留你所有原有代码，无修改）
        if ((isFinalFrame && m_isReceivingFragmentedMessage) || (opcode == 0x01 && isFinalFrame && !m_isReceivingFragmentedMessage)) {
            // 【0112下午新增】：统一处理「分片完整消息」和「非分片单帧消息」，确保业务逻辑全覆盖
            QString completeMessage;
            if (m_isReceivingFragmentedMessage) {
                completeMessage = QString::fromUtf8(m_messageFragmentCache);
                m_isReceivingFragmentedMessage = false;
                m_messageFragmentCache.clear(); // 清空分片缓存，准备下一条消息
                qDebug() << "===== 【分片】分片传输完成，完整消息长度：" << completeMessage.size() << "字符 =====";
            } else {
                completeMessage = QString::fromUtf8(framePayload); // 非分片消息直接使用当前帧payload
            }

            // ===== 以下是你原有所有逻辑，完整保留，未做任何修改 =====
            if (completeMessage.isEmpty()) continue;

            QJsonParseError jsonErr;
            QJsonDocument doc = QJsonDocument::fromJson(completeMessage.toUtf8(), &jsonErr);
            if (jsonErr.error != QJsonParseError::NoError) {
                qDebug() << "===== JSON解析失败 =====" << jsonErr.errorString();
                // 重置分片标记，避免影响下一条消息
                m_isReceivingFragmentedMessage = false;
                m_messageFragmentCache.clear();
                continue; // ❌ 替换return; 跳过当前无效帧，继续处理后续帧
            }

            QJsonObject msgObj = doc.object();
            QString msgType = msgObj["type"].toString();

            // 【0112修改】处理后端分片确认消息（精准适配后端实际返回字段，无无效字段）
            if (msgType == "file_chunk_ack") {
                // 1. 提取后端实际返回的有效字段（仅保留后端透传的字段，无多余字段）
                QString fileID = msgObj["file_id"].toString(); // 对应后端FileID（json:"file_id"），文件唯一标识

                // 【0320并发优化】提取分片编号
                QJsonValue chunkIndexValue = msgObj["chunk_index"];
                qint64 chunkIndex = chunkIndexValue.toInteger();

                QString confirmContent = msgObj["content"].toString(); // 对应后端Content，确认提示文本（可选，用于日志）

                // 2. 匹配当前正在传输的文件（使用fileID，和后端透传的FileID保持一致）
                // 注意：m_currentTransferFileId 需是客户端当前传输文件的FileID（和后端透传的msg.FileID一致）
                if (!fileID.isEmpty() && fileID == m_currentTransferFileId) {
                    // 3. 分片接收成功（后端返回的是成功提示，无需额外判断isSuccess，直接标记成功即可）
                    m_lastChunkSendSuccess = true;
                    qDebug() << "【0112新增排查】" << confirmContent << "（文件ID：" << fileID << "，分片索引：" << chunkIndex << "）";

                    // 【0320并发优化】从窗口中移除已确认的分片
                    m_inFlightChunks.remove(static_cast<int>(chunkIndex));

                    // 【0320并发优化】清理对应的超时定时器
                    if (m_chunkTimeoutTimers.contains(static_cast<int>(chunkIndex))) {
                        QTimer* timer = m_chunkTimeoutTimers[static_cast<int>(chunkIndex)];
                        if (timer) {
                            timer->stop();
                            delete timer;
                        }
                        m_chunkTimeoutTimers.remove(static_cast<int>(chunkIndex));
                    }

                    // 【0320并发优化】更新传输进度和已传输字节数
                    qint64 chunkSize = FILE_CHUNK_SIZE;
                    if (chunkIndex == m_sendTotalChunks - 1) {
                        // 最后一个分片，计算实际大小
                        chunkSize = m_fileSendTotalSize - m_fileSendHasTransferred;
                    }
                    m_fileSendHasTransferred += chunkSize;
                    updateFileTransferProgress(m_fileSendHasTransferred, m_fileSendTotalSize);

                    qDebug() << "【0320并发优化】分片" << chunkIndex << "确认成功，已传输：" << m_fileSendHasTransferred << "/" << m_fileSendTotalSize << "字节";

                    // 8. 异步触发下一个分片发送（滑动窗口逻辑）
                    QMetaObject::invokeMethod(this, "onAsyncSendNextChunk", Qt::QueuedConnection);
                } else {
                    // 可选：日志打印不匹配的文件ID，方便排查问题
                    qWarning() << "【0112新增排查】分片确认文件ID不匹配，当前传输文件ID：" << m_currentTransferFileId << "，确认文件ID：" << fileID;
                }

                // 无return，继续处理后续消息分支，不阻塞其他消息（如file_msg、chat等）
            }

            // 【0112新增】在onSslReadyRead函数的消息处理分支中，新增file_msg处理（独立if分支，不与其他分支绑定）
            // 找到mainwindow.cpp中的if (msgType == "file_msg")分支，替换为以下完整代码
            if (msgType == "file_msg") {
                QString fromUser = msgObj["from"].toString();
                QString toUser = msgObj["to"].toString();
                QString content = msgObj["content"].toString(); // JSON格式的URL集合
                qDebug() << "【0112文件URL检查】" << content;
                QString time = msgObj["time"].toString();

                // ========== 核心：会话判断（群聊/私聊完全对齐chat逻辑） ==========
                bool isGroupChat = toUser.contains("-");
                bool isCurrentSession = false;

                if (isGroupChat) {
                    qDebug() << "进入群聊分支";
                    // 群聊：只要选中的是这个群ID，就显示（不管发给谁，因为toUser是群ID）
                    isCurrentSession = (m_selectedFriend == toUser);
                } else {
                    qDebug() << "进入私聊分支";
                    // 私聊：必须是发给自己+选中的是这个好友
                    qDebug() << "当前客户端使用者为：" << m_currentUser;
                    qDebug() << "这条文件消息来自：" << fromUser;
                    qDebug() << "这条文件消息发给：" << toUser;
                    if (toUser == m_currentUser) {
                        qDebug() << "发给自己的文件消息";
                        isCurrentSession = (!m_selectedFriend.isEmpty() && (fromUser == m_selectedFriend || fromUser == m_currentUser));
                        if (!isCurrentSession) {
                            qDebug() << "[未读文件消息] 来自:" << fromUser << " 内容:" << content;
                        }
                    } else {
                        return; // 不是发给自己的私聊文件消息，直接返回
                    }
                }
                // ========== 会话判断结束 ==========

                // 解析文件附件（原有逻辑不变）
                FileAttachment fileAttach;
                QJsonDocument jsonDoc = QJsonDocument::fromJson(content.toUtf8());
                if (jsonDoc.isObject()) {
                    QJsonObject jsonObj = jsonDoc.object();
                    fileAttach.downloadUrl = jsonObj["download_url"].toString();
                    fileAttach.originUrl = jsonObj["origin_url"].toString();
                    fileAttach.previewUrl = jsonObj["preview_url"].toString();

                    fileAttach.fileName = FileUtils::extractFileName(fileAttach.originUrl.isEmpty() ? fileAttach.downloadUrl : fileAttach.originUrl);
                    fileAttach.fileSuffix = FileUtils::extractFileSuffix(fileAttach.fileName);
                    fileAttach.isImageFile = FileUtils::isImageFile(fileAttach.fileName);
                }
                
                // 保存文件消息到本地缓存（无论是否是当前会话）
                ChatMessage msg;
                msg.type = fileAttach.isImageFile ? ChatMessage::Image : ChatMessage::File;
                msg.owner = (fromUser == m_currentUser) ? ChatMessage::Self : ChatMessage::Other;
                msg.content = content;
                msg.senderName = fromUser;
                msg.receiverName = toUser;
                msg.timestamp = QDateTime::fromString(time, "yyyy-MM-dd hh:mm:ss");
                msg.isRead = isCurrentSession; // 当前会话的消息标记为已读，否则为未读
                msg.fileAttach = fileAttach;
                
                // 设置头像路径
                if (fromUser == m_currentUser) {
                    msg.avatarPath = m_myAvatarUrl;
                } else {
                    msg.avatarPath = m_friendAvatars.value(fromUser, ":/avatars/other.png");
                }
                
                // 保存到本地缓存
                LocalChatCache::instance()->saveMessage(msg);
                
                // 非当前选中会话（群/私聊），不显示但已保存到本地缓存
                if (!isCurrentSession) {
                    qDebug() << "非当前选中会话，文件消息已保存到本地缓存";
                    return;
                }
                qDebug() << "预览图URL：" << fileAttach.previewUrl;

                // ========== 关键修正：移除buildAndAddChatMessage里的私聊筛选 ==========
                auto buildAndAddChatMessage = [&](ChatMessage::MessageOwner owner, const QString &avatarUrl) {
                    // 仅保留非空判断（前面已完成所有会话筛选）
                    if (!m_selectedFriend.isEmpty()) {
                        ChatMessage msg;
                        msg.senderName = fromUser;
                        msg.content = content;
                        msg.timestamp = QDateTime::fromString(time, "yyyy-MM-dd hh:mm:ss");
                        msg.owner = owner;
                        msg.avatarPath = avatarUrl;
                        msg.fileAttach = fileAttach;
                        msg.type = fileAttach.isImageFile ? ChatMessage::Image : ChatMessage::File;

                        ChatModel *chatModel = qobject_cast<ChatModel*>(ui->chat_info_show->model());
                        if (chatModel) {
                            chatModel->addMessage(msg);
                        }

                        // 自动滚动到底部
                        QMetaObject::invokeMethod(this, [=] {
                            ui->chat_info_show->verticalScrollBar()->setValue(
                                ui->chat_info_show->verticalScrollBar()->maximum()
                                );
                        }, Qt::QueuedConnection);
                    }
                };

                // 1. 处理别人发的文件消息（群聊/私聊通用）
                if (fromUser != m_currentUser) {
                    // 群聊：取发送者的头像；私聊：取选中好友的头像
                    QString otherAvatarUrl = isGroupChat ? m_friendAvatars.value(fromUser, "") : m_friendAvatars[m_selectedFriend];
                    qDebug() << "对方头像url为" << otherAvatarUrl;
                    buildAndAddChatMessage(ChatMessage::Other, otherAvatarUrl);
                }

                // 2. 处理自己发的文件消息（群聊/私聊通用）
                if (fromUser == m_currentUser) {
                    QString selfAvatarUrl = m_myAvatarUrl;
                    buildAndAddChatMessage(ChatMessage::Self, selfAvatarUrl);
                }
            }
            // ===== 处理用户信息（头像URL）=====
            if (msgType == "user_info") {
                QString avatarUrl = msgObj["avatar"].toString();
                m_myAvatarUrl = avatarUrl;
                qDebug() << "登录用户头像URL查询为" << m_myAvatarUrl;
                if (!avatarUrl.isEmpty()) {
                    loadUserAvatar(avatarUrl); // 异步加载
                }
                m_messageFragmentCache.clear(); // 清空分片缓存
                continue;
            }

            // ===== 原有逻辑：处理好友列表 =====
            // 【0116批注：好友列表的数据量通常较小（几十、上百个好友）】
            // 【0116批注：即使每次重新加载几十条数据，对 Qt 的 Model/View 架构来说，性能开销几乎可以忽略，不会出现界面闪烁、卡顿等问题，用户无感知】
            if (msgType == "friends") {
                qDebug() << "📦 原始 friends 数据:" << msgObj;
                QList<FriendData> friendList;
                QJsonArray friendsArr = msgObj["friends"].toArray();

                qDebug() << "👥 当前好友列表:";
                for (auto it = friendsArr.constBegin(); it != friendsArr.constEnd(); ++it) {
                    const QJsonValue &val = *it;
                    if (val.isObject()) {
                        QJsonObject obj = val.toObject();
                        FriendData data;
                        data.username = obj["Username"].toString();
                        data.avatarUrl = obj["Avatar"].toString();
                        // 0106新增：缓存用户的每个好友的头像URL
                        m_friendAvatars[data.username] = data.avatarUrl; // 存到内存缓存
                        friendList.append(data);

                        // 打印好友信息
                        qDebug() << "  - 用户名:" << data.username << "头像URL:" << data.avatarUrl;
                    }
                }
                qDebug() << "👥 好友总数:" << friendList.size();

                // 将好友信息保存到本地缓存
                LocalChatCache::instance()->saveUserInfos(friendList);

                loadFriendList(friendList);
            }
            // 【0123白天未读消息计数逻辑开始：将redis统计的未读计数用map来存储——哪个好友/群聊有多少条未读，并显示红点】
            // 【0201白天心跳包逻辑开始：处理服务端返回的pong心跳响应】
            if (msgType == "pong") {
                qDebug() << "\n📌 开始解析心跳pong响应消息";
                // 1. 提取pong消息关键字段（from/server、content、time）
                QString pongFrom = msgObj["from"].toString();
                QString pongContent = msgObj["content"].toString();
                QString pongTime = msgObj["time"].toString();
                // 【0201白天QT心跳包逻辑开始：收到pong后重置超时定时器】
                if (m_pongTimeoutTimer && m_pongTimeoutTimer->isActive()) {
                    m_pongTimeoutTimer->stop();
                    qDebug() << "✅ 收到pong，已停止pong超时定时器";
                }
                // 【0201白天QT心跳包逻辑结束】
                // 2. 关键字段非空校验（兼容服务端字段缺失）
                if (pongFrom.isEmpty()) {
                    qDebug() << "❌ pong消息from字段为空，跳过解析";
                    return;
                }
                if (pongContent.isEmpty()) {
                    qDebug() << "⚠️ pong消息content字段为空，使用默认值";
                    pongContent = "pong";
                }
                if (pongTime.isEmpty()) {
                    qDebug() << "⚠️ pong消息time字段为空，使用本地时间";
                    pongTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
                }

                // 打印原始pong消息信息（方便调试）
                qDebug() << "📦 原始pong消息详情:";
                qDebug() << "   发送方：" << pongFrom;
                qDebug() << "   内容：" << pongContent;
                qDebug() << "   服务端时间：" << pongTime;
                qDebug() << "   本地接收时间：" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

                // 3. 核心心跳逻辑：重置pong超时计时器+更新连接状态
                // ======================== 需替换的变量说明 ========================
                // m_pongTimeoutTimer：客户端定义的pong超时计时器（QTimer*），用于检测服务端是否响应
                // m_heartbeatSendTimer：客户端发送ping的计时器（QTimer*），定时发ping
                // m_isWebSocketConnected：标记WebSocket连接状态的布尔变量（bool）
                // m_reconnectTimer：重连计时器（QTimer*），连接断开时触发重连（可选）
                // =================================================================
                if (m_pongTimeoutTimer) {
                    // 重置pong超时计时器（停止计时，重新开始）
                    m_pongTimeoutTimer->stop();
                    m_pongTimeoutTimer->start(60000); // 重置为60秒超时（和服务端HeartbeatTimeout一致）
                    qDebug() << "✅ 重置pong超时计时器（60秒），等待下一次ping响应";
                }

                // 更新连接状态为"正常"
                m_isWebSocketConnected = true;
                qDebug() << "✅ WebSocket连接状态更新：正常（收到服务端pong响应）";

                // 6. 汇总打印（调试用）
                qDebug() << "\n✅ pong心跳响应解析完成：";
                qDebug() << "   连接状态：正常";
                qDebug() << "   下次ping发送时间：" << (m_heartbeatSendTimer ?
                                                            m_heartbeatSendTimer->remainingTime()/1000 : 0) << "秒后";
                qDebug() << "   pong超时检测剩余时间：" << (m_pongTimeoutTimer ?
                                                                m_pongTimeoutTimer->remainingTime()/1000 : 0) << "秒";
            }
            // 【0201白天心跳包逻辑结束】
            if (msgType == "unread_count") {
                qDebug() << "\n📌 开始解析未读计数消息";
                // 1. 提取Content字段（服务端存放未读计数字典的JSON字符串）
                QString unreadCountJsonStr = msgObj["content"].toString();
                if (unreadCountJsonStr.isEmpty()) {
                    qDebug() << "❌ 未读计数Content为空，跳过解析";
                    return;
                }
                qDebug() << "📦 原始未读计数JSON:" << unreadCountJsonStr;

                // 2. 解析Content为JSON对象（键=好友名/群ID，值=未读数量字符串）
                QJsonParseError jsonErr;
                QJsonDocument countDoc = QJsonDocument::fromJson(unreadCountJsonStr.toUtf8(), &jsonErr);
                if (jsonErr.error != QJsonParseError::NoError) {
                    qDebug() << "❌ 未读计数字典解析失败：" << jsonErr.errorString();
                    return;
                }
                if (!countDoc.isObject()) {
                    qDebug() << "❌ 未读计数字典不是JSON对象，跳过";
                    return;
                }
                QJsonObject unreadCountObj = countDoc.object();

                // 3. 遍历未读计数字典，区分好友/群聊，提取未读数量
                QList<QString> friendUnreadKeys;   // 好友未读的键（用户名）
                QList<QString> groupUnreadKeys;    // 群聊未读的键（群ID）
                QMap<QString, int> friendUnreadMap;// 好友未读映射：用户名→未读数量
                QMap<QString, int> groupUnreadMap; // 群聊未读映射：群ID→未读数量

                // 遍历所有键值对
                for (auto it = unreadCountObj.constBegin(); it != unreadCountObj.constEnd(); ++it) {
                    QString key = it.key();          // 好友名/群ID
                    QString countStr = it.value().toString(); // 未读数量（字符串）

                    // 转换未读数量为整数（兼容空值/非数字）
                    bool isNumber = false;
                    int unreadCount = countStr.toInt(&isNumber);
                    if (!isNumber) {
                        qDebug() << "⚠️ 键[" << key << "]的未读数量不是有效数字：" << countStr << "，默认设为0";
                        unreadCount = 0;
                    }

                    // 区分好友/群聊（群ID含"-"，与服务端群ID规则一致）
                    if (key.contains("-")) {
                        // 群聊未读
                        groupUnreadKeys.append(key);
                        groupUnreadMap.insert(key, unreadCount);
                        qDebug() << "👥 群聊未读：群ID=[" << key << "]，未读数量=" << unreadCount;
                    } else {
                        // 好友私聊未读
                        friendUnreadKeys.append(key);
                        friendUnreadMap.insert(key, unreadCount);
                        qDebug() << "👤 好友未读：用户名=[" << key << "]，未读数量=" << unreadCount;
                    }
                }

                // 4. 汇总打印（方便调试）
                qDebug() << "\n✅ 未读计数解析完成：";
                qDebug() << "   有未读的好友数：" << friendUnreadMap.size();
                qDebug() << "   有未读的群聊数：" << groupUnreadMap.size();
                qDebug() << "   好友未读明细：" << friendUnreadMap;
                qDebug() << "   群聊未读明细：" << groupUnreadMap;
                // 【0127白天未读消息实时更新逻辑开始：优化选中好友/群聊的未读处理】
                bool isSelectedTarget = false;
                QString targetKey = "";
                // 思路1：修正判断条件 - 包含key 且 未读数>0 才触发逻辑
                // 检查是否是选中的好友（未读数>0）
                if (friendUnreadMap.contains(m_selectedFriend) && friendUnreadMap.value(m_selectedFriend) > 0) {
                    isSelectedTarget = true;
                    targetKey = m_selectedFriend;
                }
                // 检查是否是选中的群聊（未读数>0）
                else if (groupUnreadMap.contains(m_selectedFriend) && groupUnreadMap.value(m_selectedFriend) > 0) {
                    isSelectedTarget = true;
                    targetKey = m_selectedFriend;
                }

                if (isSelectedTarget) {
                    qDebug() << "ℹ️ 当前选中的好友/群聊[" << m_selectedFriend << "]有未读消息，开始清除计数并更新红点";

                    // 构造clear_unread_count消息发送给后端
                    QJsonObject clearMsgObj;
                    clearMsgObj["type"] = "clear_unread_count";
                    clearMsgObj["from"] = m_currentUser; // 替换为实际登录用户名变量
                    clearMsgObj["to"] = targetKey;       // 要清除未读计数的好友/群ID
                    clearMsgObj["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
                    sendWebSocketMessage(QJsonDocument(clearMsgObj).toJson(QJsonDocument::Compact));

                    // 思路2：主动置0 - 手动将前端该对象的未读数设为0，不跳过更新逻辑
                    if (friendUnreadMap.contains(targetKey)) {
                        friendUnreadMap.insert(targetKey, 0); // 好友未读数置0
                    } else if (groupUnreadMap.contains(targetKey)) {
                        groupUnreadMap.insert(targetKey, 0); // 群聊未读数置0
                    }

                    // 移除原有的return，继续执行后续的红点更新逻辑
                    qDebug() << "✅ 已将选中对象的未读数置0，将继续更新红点";
                }
                // 【0127白天未读消息实时更新逻辑结束】
                // 【0123白天未读消息逻辑开始：更新好友列表模型的未读数量】
                // 假设你的FriendListModel实例是m_friendListModel（需替换为实际变量名）
                // MainWindow中更新好友未读数量的代码后，新增：
                if (m_friendListModel) {
                    for (auto it = friendUnreadMap.constBegin(); it != friendUnreadMap.constEnd(); ++it) {
                        QString username = it.key();
                        int count = it.value();
                        m_friendListModel->setFriendUnreadCount(username, count);

                        // 找到对应用户名的行号，强制刷新该行（正确的View更新方式）
                        int row = -1;
                        for (int i = 0; i < m_friendListModel->rowCount(); ++i) {
                            QModelIndex idx = m_friendListModel->index(i, 0);
                            if (idx.data(FriendListModel::UsernameRole).toString() == username) {
                                row = i;
                                break;
                            }
                        }
                        if (row >= 0) {
                            QRect rowRect = ui->person_list->visualRect(m_friendListModel->index(row, 0));
                            ui->person_list->viewport()->update(rowRect); // 仅刷新该行，性能最优
                        }
                    }
                }
                // 【0123白天未读消息逻辑结束】

                // 【0123白天未读消息逻辑开始：更新群聊列表模型的未读数量】
                // 假设你的GroupListModel实例是m_groupListModel（需替换为实际变量名）
                if (m_groupListModel) {
                    // 遍历群聊未读映射，更新每个群聊的未读数量
                    for (auto it = groupUnreadMap.constBegin(); it != groupUnreadMap.constEnd(); ++it) {
                        QString groupId = it.key();
                        int count = it.value();
                        m_groupListModel->setGroupUnreadCount(groupId, count);
                    }
                }
                // 【0123白天未读消息逻辑结束】
                // 假设你的好友列表委托指针是m_friendDelegate（需提前初始化）
                if (m_friendDelegate) {
                    for (auto it = friendUnreadMap.constBegin(); it != friendUnreadMap.constEnd(); ++it) {
                        QString username = it.key();
                        int count = it.value();
                        // 直接更新委托的缓存，触发重绘
                        m_friendDelegate->updateFriendUnreadCount(username, count);
                    }
                }

                if (m_groupListDelegate) {
                    for (auto it = groupUnreadMap.constBegin(); it != groupUnreadMap.constEnd(); ++it) {
                        QString username = it.key();
                        int count = it.value();
                        // 直接更新委托的缓存，触发重绘
                        m_groupListDelegate->updateGroupUnreadCount(username, count);
                    }
                }

                // 【0123未读消息逻辑开始：计算私聊/群聊总未读，调用红点更新函数】
                // 1. 计算私聊总未读：所有好友未读数量求和
                int totalChatUnread = 0;
                for (int count : friendUnreadMap.values()) {
                    totalChatUnread += count;
                }

                // 2. 计算群聊总未读：所有群聊未读数量求和
                int totalGroupUnread = 0;
                for (int count : groupUnreadMap.values()) {
                    totalGroupUnread += count;
                }

                // 3. 调用函数更新私聊/群聊按钮红点
                showChatGroupChatRedDot(totalChatUnread, totalGroupUnread);
                // 【0123未读消息逻辑结束】
            }
            // 【0123白天未读消息计数逻辑结束】

            // 【0120白天群聊列表渲染相关逻辑开始：处理group_info类型消息，填充群聊列表】
            if (msgType == "group_info")
            {
                // 清空原有群聊列表
                m_groupListModel->clearGroups();

                // 关键修改：先解析Content字段中的JSON字符串（群列表）
                QString groupsJsonStr = msgObj["content"].toString();
                if (groupsJsonStr.isEmpty())
                {
                    qWarning() << "群聊信息content字段为空，无法解析";
                    return;
                }

                // 将Content中的JSON字符串转为JSON数组
                QJsonParseError jsonError;
                QJsonDocument groupDoc = QJsonDocument::fromJson(groupsJsonStr.toUtf8(), &jsonError);
                if (jsonError.error != QJsonParseError::NoError || !groupDoc.isArray())
                {
                    qWarning() << "群聊列表JSON解析失败：" << jsonError.errorString();
                    return;
                }

                // 解析群聊列表数据
                QJsonArray groupArray = groupDoc.array();
                QList<GroupInfo> groupInfos;
                for (int i = 0; i < groupArray.size(); ++i)
                {
                    QJsonObject groupObj = groupArray[i].toObject();
                    GroupInfo groupInfo;
                    groupInfo.groupId = groupObj["group_id"].toString();
                    groupInfo.groupName = groupObj["group_name"].toString();
                    groupInfo.groupAvatar = groupObj["group_avatar"].toString();
                    groupInfo.members = groupObj["members"].toString();
                    // 添加到群聊模型，更新视图
                    m_groupListModel->addGroup(groupInfo);
                    groupInfos.append(groupInfo);
                }

                // 将群聊信息保存到本地缓存
                LocalChatCache::instance()->saveGroupInfos(groupInfos);

                qInfo() << "成功解析并加载" << groupArray.size() << "个群聊信息";
            }
            // 【0120白天群聊列表渲染相关逻辑结束】


            // ===== 原有逻辑：处理新好友请求 =====
            if (msgType == "new_friend_request") {
                FriendRequestData requestData;
                requestData.fromUser = msgObj["from"].toString();
                requestData.avatarUrl = msgObj["avatar"].toString();
                requestData.message = msgObj["content"].toString();
                m_friendRequestModel->addRequest(requestData);
                // 核心修改：重新加载请求列表，自动统计未处理数量
                loadFriendRequests();
            }
            // ===== 【0112修正】处理私聊聊天消息（修正：仅显示当前选中好友的消息）=====
            if (msgType == "chat") {
                QString fromUser = msgObj["from"].toString();  // 发送者
                QString toUser   = msgObj["to"].toString();    // 接收者
                QString content  = msgObj["content"].toString();// 消息内容
                QString time     = msgObj["time"].toString();  // 发送时间

                qDebug() << "[收到私聊消息] 来自:" << fromUser << " 发给:" << toUser << " 内容:" << content << " 时间:" << time;

                // 核心修改：仅区分群聊/私聊，不碰任何头像逻辑
                bool isGroupChat = toUser.contains("-");
                bool isCurrentSession = false;

                if (isGroupChat) {
                    // 群聊：只要当前选中的是这个群ID，就显示
                    isCurrentSession = (m_selectedFriend == toUser);
                } else {
                    // 私聊：完全还原你原来的逻辑
                    if (toUser == m_currentUser) {
                        if (!m_selectedFriend.isEmpty() && fromUser == m_selectedFriend) {
                            isCurrentSession = true;
                        } else {
                            qDebug() << "[未读消息] 来自:" << fromUser << " 内容:" << content;
                        }
                    } else {
                        return;
                    }
                }

                // 保存消息到本地缓存（无论是否是当前会话）
                ChatMessage msg;
                msg.type = ChatMessage::Text;
                msg.owner = (fromUser == m_currentUser) ? ChatMessage::Self : ChatMessage::Other;
                msg.content = content;
                msg.senderName = fromUser;
                msg.receiverName = toUser;
                msg.timestamp = QDateTime::fromString(time, "yyyy-MM-dd hh:mm:ss");
                msg.isRead = isCurrentSession; // 当前会话的消息标记为已读，否则为未读
                
                // 设置头像路径
                if (fromUser == m_currentUser) {
                    msg.avatarPath = m_myAvatarUrl;
                } else {
                    msg.avatarPath = m_friendAvatars.value(fromUser, ":/avatars/other.png");
                }
                
                // 保存到本地缓存
                LocalChatCache::instance()->saveMessage(msg);

                if (isCurrentSession) {
                    addMessageToChatShow(fromUser, content, time);

                    // 自动滚动到底部（完全还原）
                    QMetaObject::invokeMethod(this, [=]() {
                        ui->chat_info_show->verticalScrollBar()->setValue(
                            ui->chat_info_show->verticalScrollBar()->maximum()
                            );
                    }, Qt::QueuedConnection);
                }
            }
            // ========== 0108新增：处理聊天记录返回 ==========
            if (msgType == "chat_records") {
                qDebug() << "📥 收到聊天记录原始数据：" << msgObj;

                // 步骤1：获取content字段的QJsonValue，先做空值校验
                QJsonValue contentVal = msgObj["content"];
                if (contentVal.isNull() || contentVal.isUndefined()) {
                    qWarning() << "⚠️ 聊天记录content字段为空";
                    return;
                }

                QJsonArray recordsArray;
                // 步骤2：兼容两种格式（后端直接返回数组 / 数组转字符串）
                if (contentVal.isArray()) {
                    recordsArray = contentVal.toArray();
                } else if (contentVal.isString()) {
                    QString contentStr = contentVal.toString();
                    QJsonParseError parseErr;
                    QJsonDocument doc = QJsonDocument::fromJson(contentStr.toUtf8(), &parseErr);
                    if (parseErr.error != QJsonParseError::NoError) {
                        qCritical() << "❌ 解析聊天记录字符串失败：" << parseErr.errorString();
                        return;
                    }
                    if (doc.isArray()) {
                        recordsArray = doc.array();
                    } else {
                        qWarning() << "⚠️ 聊天记录字符串不是数组格式";
                        return;
                    }
                } else {
                    qWarning() << "⚠️ 聊天记录content字段类型错误（非数组/字符串）";
                    return;
                }

                // 【新增：提取公共逻辑1 - 解析content为FileAttachment（和file_msg逻辑一致）】
                auto parseContentToFileAttach = [](const QString &content) -> FileAttachment {
                    FileAttachment fileAttach;
                    QJsonDocument jsonDoc = QJsonDocument::fromJson(content.toUtf8());
                    if (jsonDoc.isObject()) {
                        QJsonObject jsonObj = jsonDoc.object();
                        fileAttach.downloadUrl = jsonObj["download_url"].toString();
                        fileAttach.originUrl = jsonObj["origin_url"].toString();
                        fileAttach.previewUrl = jsonObj["preview_url"].toString();

                        // 提取文件元信息，判断是否为图片文件（复用FileUtils工具类）
                        fileAttach.fileName = FileUtils::extractFileName(fileAttach.originUrl.isEmpty() ? fileAttach.downloadUrl : fileAttach.originUrl);
                        fileAttach.fileSuffix = FileUtils::extractFileSuffix(fileAttach.fileName);
                        fileAttach.isImageFile = FileUtils::isImageFile(fileAttach.fileName);
                    }
                    return fileAttach;
                };

                // 【仅修改筛选逻辑，头像逻辑完全还原你原来的代码】
                [[maybe_unused]] auto buildAndAddChatRecordMessage = [&](const QString &fromUser, const QString &msgTo, const QString &content, const QString &time, FileAttachment &fileAttach) {
                    // 第一步：判断当前选中的是否是群聊（群ID含"-"，用户名无"-"无干扰）
                    bool isGroupChat = !m_selectedFriend.isEmpty() && m_selectedFriend.contains("-");

                    // 第二步：确定消息是否属于当前选中的会话
                    bool isCurrentSession = false;
                    if (isGroupChat) {
                        // 群聊：消息的to字段是群ID，只要和选中的群ID一致，就显示
                        isCurrentSession = (msgTo == m_selectedFriend);
                    } else {
                        // 私聊：完全还原你原来的筛选逻辑，一字不改
                        isCurrentSession = !m_selectedFriend.isEmpty() &&
                                           ((fromUser == m_selectedFriend) || (fromUser == m_currentUser && m_selectedFriend == msgTo));
                    }

                    if (!isCurrentSession) {
                        return; // 不属于当前会话，跳过
                    }

                    // ========== 头像逻辑100%还原你原来的代码，未做任何修改 ==========
                    // 确定消息所有者（自己/对方）和对应的头像URL
                    ChatMessage::MessageOwner owner = (fromUser == m_currentUser) ? ChatMessage::Self : ChatMessage::Other;
                    QString avatarUrl = (owner == ChatMessage::Self) ? m_myAvatarUrl : m_friendAvatars.value(fromUser, "");

                    // 仅处理当前选中好友的聊天记录（和file_msg筛选逻辑一致）
                    // 注：此处isCurrentSession已做筛选，该判断可保留但不影响，完全还原你原逻辑
                    if (!m_selectedFriend.isEmpty() &&
                        (isGroupChat ||
                         ((owner == ChatMessage::Other && fromUser == m_selectedFriend) ||
                          (owner == ChatMessage::Self && m_selectedFriend == m_selectedFriend)))) {
                        // 构建ChatMessage对象（和file_msg消息结构一致）
                        ChatMessage msg;
                        msg.senderName = fromUser;
                        msg.content = content;
                        msg.timestamp = QDateTime::fromString(time, "yyyy-MM-dd hh:mm:ss");
                        msg.owner = owner;
                        msg.avatarPath = avatarUrl;
                        msg.fileAttach = fileAttach;
                        // 区分消息类型（图片/文件/普通文本）
                        if (fileAttach.isImageFile) {
                            msg.type = ChatMessage::Image;
                        } else if (!fileAttach.downloadUrl.isEmpty() || !fileAttach.originUrl.isEmpty()) {
                            msg.type = ChatMessage::File;
                        } else {
                            msg.type = ChatMessage::Text;
                        }

                        // 添加到ChatModel（替换原有addMessageToChatShow，和file_msg统一）
                        ChatModel *chatModel = qobject_cast<ChatModel*>(ui->chat_info_show->model());
                        if (chatModel) {
                            chatModel->addMessage(msg);
                        }
                    }
                };

                // 检查是否是懒加载请求（如果有消息且包含加载提示，说明是懒加载）
                bool isLazyLoad = false;
                ChatModel *chatModel = qobject_cast<ChatModel*>(ui->chat_info_show->model());
                if (chatModel && chatModel->rowCount() > 0) {
                    QVector<ChatMessage> messages = chatModel->messages();
                    if (!messages.isEmpty() && messages.last().owner == ChatMessage::System && 
                        messages.last().content == "正在加载更多聊天记录...") {
                        isLazyLoad = true;
                        // 移除加载提示
                        chatModel->removeLastMessage();
                    }
                }
                
                // 收集所有历史消息
                QVector<ChatMessage> historyMessages;
                
                // 步骤3：遍历聊天记录，解析并构建带图片/文件的消息
                for (QJsonArray::const_iterator it = recordsArray.cbegin(); it != recordsArray.cend(); ++it) {
                    const QJsonValue& val = *it;

                    if (!val.isObject()) {
                        qWarning() << "⚠️ 聊天记录项不是对象：" << val;
                        continue;
                    }
                    QJsonObject record = val.toObject();
                    QString fromUser = record["from"].toString().trimmed();
                    QString msgTo = record["to"].toString().trimmed(); // 仅新增提取msgTo字段
                    QString content = record["content"].toString().trimmed();
                    QString time = record["time"].toString().trimmed();

                    // 空值过滤（补充msgTo，不影响原有逻辑）
                    if (fromUser.isEmpty() || content.isEmpty()) continue;

                    // 【完全还原你原来的代码】
                    FileAttachment fileAttach = parseContentToFileAttach(content);
                    qDebug() << "聊天记录图片预览URL：" << fileAttach.previewUrl;
                    
                    // 第一步：判断当前选中的是否是群聊（群ID含"-",用户名无"-"无干扰）
                    bool isGroupChat = !m_selectedFriend.isEmpty() && m_selectedFriend.contains("-");

                    // 第二步：确定消息是否属于当前选中的会话
                    bool isCurrentSession = false;
                    if (isGroupChat) {
                        // 群聊：消息的to字段是群ID，只要和选中的群ID一致，就显示
                        isCurrentSession = (msgTo == m_selectedFriend);
                    } else {
                        // 私聊：完全还原你原来的筛选逻辑，一字不改
                        isCurrentSession = !m_selectedFriend.isEmpty() &&
                                           ((fromUser == m_selectedFriend) || (fromUser == m_currentUser && m_selectedFriend == msgTo));
                    }

                    if (!isCurrentSession) {
                        continue; // 不属于当前会话，跳过
                    }

                    // ========== 头像逻辑100%还原你原来的代码，未做任何修改 ==========
                    // 确定消息所有者（自己/对方）和对应的头像URL
                    ChatMessage::MessageOwner owner = (fromUser == m_currentUser) ? ChatMessage::Self : ChatMessage::Other;
                    QString avatarUrl = (owner == ChatMessage::Self) ? m_myAvatarUrl : m_friendAvatars.value(fromUser, "");

                    // 仅处理当前选中好友的聊天记录（和file_msg筛选逻辑一致）
                    // 注：此处isCurrentSession已做筛选，该判断可保留但不影响，完全还原你原逻辑
                    if (!m_selectedFriend.isEmpty() &&
                        (isGroupChat ||
                         ((owner == ChatMessage::Other && fromUser == m_selectedFriend) ||
                          (owner == ChatMessage::Self && m_selectedFriend == m_selectedFriend)))) {
                        // 构建ChatMessage对象（和file_msg消息结构一致）
                        ChatMessage msg;
                        msg.senderName = fromUser;
                        msg.content = content;
                        msg.timestamp = QDateTime::fromString(time, "yyyy-MM-dd hh:mm:ss");
                        msg.owner = owner;
                        msg.avatarPath = avatarUrl;
                        msg.fileAttach = fileAttach;
                        // 区分消息类型（图片/文件/普通文本）
                        if (fileAttach.isImageFile) {
                            msg.type = ChatMessage::Image;
                        } else if (!fileAttach.downloadUrl.isEmpty() || !fileAttach.originUrl.isEmpty()) {
                            msg.type = ChatMessage::File;
                        } else {
                            msg.type = ChatMessage::Text;
                        }
                        
                        if (isLazyLoad) {
                            // 懒加载时，收集消息（注意：后端返回的是按时间升序排列的历史消息）
                            historyMessages.append(msg);
                        } else {
                            // 初始加载时，云端返回的是最新的20条消息
                            // 我们需要：1. 保存到本地缓存 2. 替换当前显示的消息
                            // 设置接收者名称和已读状态
                            msg.receiverName = msgTo;
                            msg.isRead = true; // 因为已经显示在界面上了，标记为已读
                            
                            // 保存到本地缓存
                            LocalChatCache::instance()->saveMessage(msg);
                            
                            // 先收集所有云端消息
                            historyMessages.append(msg);
                        }
                    }
                }
                
                // 处理收集到的消息
                if (chatModel) {
                    if (isLazyLoad) {
                        // 懒加载时，批量添加历史消息到开头
                        // 保存当前第一个可见项的索引，用于恢复滚动位置
                        QModelIndex firstVisibleIndex = ui->chat_info_show->indexAt(QPoint(0, 0));
                        int firstVisibleRow = firstVisibleIndex.isValid() ? firstVisibleIndex.row() : 0;
                        
                        // 保存懒加载的消息到本地缓存
                        for (auto &msg : historyMessages) {
                            msg.receiverName = m_selectedFriend;
                            msg.isRead = true; // 因为已经显示在界面上了，标记为已读
                            LocalChatCache::instance()->saveMessage(msg);
                        }
                        
                        // 添加新消息到开头
                        chatModel->prependMessages(historyMessages);
                        
                        // 恢复滚动位置：滚动到原来可见的第一项（现在索引增加了 historyMessages.size()）
                        // 使用 QMetaObject::invokeMethod 确保在布局更新后执行
                        int newFirstVisibleRow = firstVisibleRow + historyMessages.size();
                        QMetaObject::invokeMethod(this, [=] {
                            QModelIndex targetIndex = chatModel->index(newFirstVisibleRow, 0);
                            if (targetIndex.isValid()) {
                                ui->chat_info_show->scrollTo(targetIndex, QAbstractItemView::PositionAtTop);
                            }
                        }, Qt::QueuedConnection);
                        
                        // 如果加载的消息数量少于请求数量，说明没有更多历史消息了
                        if (historyMessages.size() < 20) {
                            chatModel->setHasMoreMessages(false);
                        }
                        
                        // 重置加载状态
                        m_isLoading = false;
                    } else {
                        // 初始加载时，只添加云端返回的新消息，不替换现有消息
                        if (!historyMessages.isEmpty()) {
                            // 按时间正序添加新消息
                            std::sort(historyMessages.begin(), historyMessages.end(), [](const ChatMessage &a, const ChatMessage &b) {
                                return a.timestamp < b.timestamp;
                            });
                            
                            for (const auto &msg : historyMessages) {
                                // 只添加本地没有的新消息
                                bool exists = false;
                                QVector<ChatMessage> existingMessages = chatModel->messages();
                                for (const auto &existingMsg : existingMessages) {
                                    if (existingMsg.timestamp == msg.timestamp && existingMsg.content == msg.content) {
                                        exists = true;
                                        break;
                                    }
                                }
                                
                                if (!exists) {
                                    chatModel->addMessage(msg);
                                }
                            }
                            
                            qDebug() << "✅ 添加了云端最新的" << historyMessages.size() << "条消息";
                        }
                    }
                }

                // 自动滚动到底部（仅在首次加载且没有本地消息时）
                if (!isLazyLoad && chatModel->rowCount() <= historyMessages.size()) {
                    // 只有在真正的初始加载时（模型中没有其他消息）才滚动到底部
                    // 避免在用户已经手动滚动后又被强制拉到最新消息
                    QMetaObject::invokeMethod(this, [=] { 
                        ui->chat_info_show->verticalScrollBar()->setValue(
                            ui->chat_info_show->verticalScrollBar()->maximum()
                            );
                    }, Qt::QueuedConnection);
                }
            }
            // ===== 分片消息处理完成，重置标记 =====
            m_isReceivingFragmentedMessage = false;
        }
    }
}

void MainWindow::loadUserAvatar(const QString &avatarUrl)
{
    QNetworkRequest request{QUrl(avatarUrl)};

    // 请求头
    request.setRawHeader("User-Agent", "curl/7.29.0");
    request.setRawHeader("Host", "www.singchat.chat");
    request.setRawHeader("Accept", "*/*");

    // SSL 配置
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setProtocol(QSsl::TlsV1_2);
    sslConfig.setSslOption(QSsl::SslOptionDisableServerNameIndication, false);

    QList<QSslCipher> ciphers = sslConfig.ciphers();
    ciphers << QSslCipher("ECDHE-RSA-AES128-GCM-SHA256");
    sslConfig.setCiphers(ciphers);

    request.setSslConfiguration(sslConfig);

    QNetworkReply *reply = m_netManager->get(request);

    // SSL错误处理
    connect(reply, &QNetworkReply::sslErrors, this, [=](const QList<QSslError> &errors) {
        qDebug() << "⚠️ SSL 错误：" << errors;
        reply->ignoreSslErrors();
    });

    // 请求完成后调用 onAvatarLoaded
    connect(reply, &QNetworkReply::finished, this, [=]() {
        onAvatarLoaded(reply);
    });
}



void MainWindow::onAvatarLoaded(QNetworkReply *reply)
{
    QByteArray data = reply->readAll();
    QImage image;
    if (image.loadFromData(data)) {
        qDebug() << "✅ 头像加载成功，尺寸：" << image.size();

        // 确保在UI线程更新
        QMetaObject::invokeMethod(this, [=]() {
            // 1. 清空按钮文字，避免文字占用空间
            ui->userAvatar->setText("");

            // 2. 去掉内边距，让图片紧贴按钮边缘
            ui->userAvatar->setStyleSheet("QPushButton { padding: 0px; border: none; }");

            // 3. 忽略比例，拉伸铺满整个按钮
            ui->userAvatar->setIcon(QPixmap::fromImage(image.scaled(
                ui->userAvatar->size(),
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation
                )));

            // 4. 让图标大小等于按钮大小
            ui->userAvatar->setIconSize(ui->userAvatar->size());
        }, Qt::QueuedConnection);
    } else {
        qCritical() << "❌ 头像加载失败";
    }

    reply->deleteLater();
}

void MainWindow::loadFriendList(const QList<FriendData> &friendList)
{
    QMetaObject::invokeMethod(this, [=]() {
        m_friendListModel->clearFriends();
        for (const auto &data : friendList) {
            m_friendListModel->addFriend(data);
        }
    }, Qt::QueuedConnection);
}

// ==============0108聊天记录功能新增改动====================
void MainWindow::onPersonListItemClicked(const QModelIndex &index)
{
    if (!index.isValid()) return;
    m_selectedFriend = index.data(FriendListModel::UsernameRole).toString().trimmed();
    // ========== 【0117下午新增】设置QLabel的文本为选中的好友名称 ==========
    ui->person_name->setText(m_selectedFriend);
    // 原来的 QTextEdit 清空 → 替换为模型清空
    m_chatModel->clearMessages(); // 清空聊天模型数据
    ui->chat_info_show->reset();  // 可选：刷新视图，确保界面同步清空

    // ========== 1. 初始加载：只显示最新的20条消息 ==========
    // 先清空聊天模型
    m_chatModel->clearMessages();
    
    // 从本地缓存加载最新的20条消息
    QVector<ChatMessage> allMessages = LocalChatCache::instance()->getChatRecords(
        m_currentUser, m_selectedFriend, 20
    );
    
    // 将本地消息添加到聊天模型
    for (const auto &msg : allMessages) {
        ChatMessage msgCopy = msg;
        msgCopy.receiverName = m_selectedFriend;
        if (msg.owner == ChatMessage::Self) {
            msgCopy.avatarPath = m_myAvatarUrl;
        } else {
            msgCopy.avatarPath = m_friendAvatars.value(msg.senderName, "");
        }
        m_chatModel->addMessage(msgCopy);
    }
    
    qDebug() << "✅ 初始加载了" << allMessages.size() << "条聊天记录";

    // ========== 2. 从云端获取可能的新消息 ==========
    QJsonObject reqObj;
    reqObj["type"] = "get_chat_records";  // 对应Go后端的聊天记录查询路由
    reqObj["from"] = m_currentUser;       // 当前登录用户
    reqObj["to"] = m_selectedFriend;      // 选中的好友
    reqObj["pull_num"] = 20;  // 只请求最新的20条消息
    
    // 直接请求最新的20条消息，不指定时间戳，让后端返回最新的20条
    reqObj["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    // 复用你现有发送函数
    sendWebSocketMessage(QJsonDocument(reqObj).toJson(QJsonDocument::Compact));
    qDebug() << "🔍 发送查询聊天记录请求：" << m_currentUser << " ↔ " << m_selectedFriend;
}


// 【0120晚上新增群聊列表点击事件】
void MainWindow::onGroupListItemClicked(const QModelIndex &index)
{
    if (!index.isValid()) return;
    // 【0120晚上群聊聊天记录逻辑开始】
    // 修正：获取群ID（核心！群聊消息的to字段存储群ID）
    QString groupId = index.data(GroupListModel::GroupIdRole).toString().trimmed();
    // 群聊名称用于界面显示
    QString groupName = index.data(GroupListModel::GroupNameRole).toString().trimmed();
    // 群聊场景下，m_selectedFriend 存储群ID（和后端群聊消息的to字段对齐）
    m_selectedFriend = groupId;
    // 【0120晚上群聊聊天记录逻辑结束】

    ui->person_name->setText(groupName);

    m_chatModel->clearMessages(); // 清空聊天模型数据，不要误写成清空群聊列表数据，这样你点一下选项群聊列表直接没了
    ui->chat_info_show->reset();  // 可选：刷新视图，确保界面同步清空

    // ========== 1. 初始加载：只显示最新的20条群聊记录 ==========
    // 从本地缓存加载最新的20条消息
    QVector<ChatMessage> allMessages = LocalChatCache::instance()->getChatRecords(
        m_currentUser, m_selectedFriend, 20
    );
    
    // 将本地消息添加到聊天模型
    for (const auto &msg : allMessages) {
        ChatMessage msgCopy = msg;
        msgCopy.receiverName = m_selectedFriend;
        if (msg.owner == ChatMessage::Self) {
            msgCopy.avatarPath = m_myAvatarUrl;
        } else {
            msgCopy.avatarPath = m_friendAvatars.value(msg.senderName, "");
        }
        m_chatModel->addMessage(msgCopy);
    }
    
    qDebug() << "✅ 初始加载了" << allMessages.size() << "条群聊记录";

    // ========== 2. 从云端获取可能的新群聊消息 ==========
    QJsonObject reqObj;
    reqObj["type"] = "get_chat_records";  // 对应Go后端的聊天记录查询路由
    reqObj["from"] = m_currentUser;       // 当前登录用户
    reqObj["to"] = m_selectedFriend;      // 群聊场景下：to=群ID；私聊场景下：to=好友名
    reqObj["pull_num"] = 20;  // 只请求最新的20条消息
    
    // 直接请求最新的20条消息，不指定时间戳，让后端返回最新的20条
    reqObj["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    // 复用你现有发送函数
    sendWebSocketMessage(QJsonDocument(reqObj).toJson(QJsonDocument::Compact));
    qDebug() << "🔍 发送查询群聊记录请求：" << m_currentUser << " ↔ " << m_selectedFriend;
}

void MainWindow::onFriendRequestListItemClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    m_selectedRequestFromUser = index.data(FriendRequestModel::FromUserRole).toString().trimmed();
    FriendRequestData reqData = m_friendRequestModel->getRequest(index.row());
    // ========== 新增：显示标签 ==========
    ui->cheakinfoArea->show();
    ui->nameArea->show();
    ui->photoArea->show();
    // ========== 同时保留原有的按钮显示 ==========
    ui->btnAgree->show();
    ui->btnRefuse->show();
    // ==========0105新增：初始隐藏好友同意状态文本
    ui->Friend_status->hide();
    ui->btnAgree->setEnabled(true);
    ui->btnRefuse->setEnabled(true);

    // ==========0105新增根据状态更新UI
    if (reqData.status == 0) { // 待处理
        ui->btnAgree->show();
        ui->btnRefuse->show();
        ui->btnAgree->setEnabled(true);
        ui->btnRefuse->setEnabled(true);
    } else if (reqData.status == 1) { // 已同意
        ui->Friend_status->show();
        ui->btnAgree->hide();
        ui->btnAgree->setEnabled(false);
        ui->btnRefuse->hide();
        ui->btnRefuse->setEnabled(false);
        ui->Friend_status->setText("您已同意好友请求");
    } else if (reqData.status == 2) { // 已拒绝
        ui->Friend_status->show();
        ui->btnAgree->hide();
        ui->btnAgree->setEnabled(false);
        ui->btnRefuse->hide();
        ui->btnRefuse->setEnabled(false);
        ui->Friend_status->setText("您已拒绝好友请求");
    }

    qDebug() << "📩 [onFriendRequestListItemClicked] 请求数据："
             << "fromUser=" << reqData.fromUser
             << "avatarUrl=" << reqData.avatarUrl
             << "message=" << reqData.message;

    // 加载头像、设置文本的逻辑
    QNetworkRequest req(QUrl(reqData.avatarUrl));
    req.setSslConfiguration(m_sslConf);
    QNetworkReply *reply = m_netManager->get(req);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray data = reply->readAll();
        QImage image;
        if (image.loadFromData(data)) {
            qDebug() << "✅ [onFriendRequestListItemClicked] 头像加载成功";
            QPixmap pixmap = QPixmap::fromImage(image).scaled(
                ui->photoArea->size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                );
            ui->photoArea->setPixmap(pixmap);
        } else {
            qCritical() << "❌ [onFriendRequestListItemClicked] 头像加载失败";
        }
        reply->deleteLater();
    });

    ui->nameArea->setText(reqData.fromUser);
    ui->cheakinfoArea->setText(reqData.message);
}


void MainWindow::onBtnAddFriendsClicked()
{
    if (!m_addFriendsDialog) {
        // 确保AddFriends构造函数为explicit AddFriends(QWidget *parent = nullptr)
        m_addFriendsDialog = new AddFriends(m_netManager, m_sslConf, m_currentUser, this);
    }
    m_addFriendsDialog->show();
}

void MainWindow::onBtnStartChatClicked()
{
    ui->stackedWidget3->setCurrentWidget(ui->stackedWidget3_chat);
    ui->stackedWidget2->setCurrentWidget(ui->stackedWidget2_chat);
    // 【0116晚上修改：每次点击聊天按钮，都重新拉取一次好友列表，解决发出添加好友申请的人，没法拉到新好友列表的问题】
    // 【0116晚上批注：添加好友放到联系人列表那里了，所以添加完联系人，你肯定要点击聊天的好友列表的，就顺势刷新新的联系人了，逻辑上更加合理一些】
    QJsonObject reqMsg;
    reqMsg["type"] = "get_friends";
    reqMsg["from"] = m_currentUser;
    sendWebSocketMessage(QJsonDocument(reqMsg).toJson(QJsonDocument::Compact));

    QJsonObject unread_msg;
    unread_msg["type"] = "get_unread_count";
    unread_msg["from"] = m_currentUser;
    sendWebSocketMessage(QJsonDocument(unread_msg).toJson(QJsonDocument::Compact));
}

// 【0120白天修改】
void MainWindow::ongroupchatClicked()
{
    ui->stackedWidget3->setCurrentWidget(ui->stackedWidget3_chat);
    ui->stackedWidget2->setCurrentWidget(ui->stackedWidget2_group);

    qDebug() << "触发群聊按钮点击";
    // 构造get_group_info类型消息（格式仿照好友列表请求，根据项目实际消息格式调整）
    QJsonObject msgObj;
    msgObj["type"] = "get_group_info";
    msgObj["from"] = m_currentUser; // 与后端的from字段对齐
    sendWebSocketMessage(QJsonDocument(msgObj).toJson(QJsonDocument::Compact));

    QJsonObject msgObj_group_unread;
    msgObj_group_unread["type"] = "get_unread_count";
    msgObj_group_unread["from"] = m_currentUser; // 与后端的from字段对齐
    sendWebSocketMessage(QJsonDocument(msgObj_group_unread).toJson(QJsonDocument::Compact));
}

void MainWindow::onBtnAgreeClicked()
{
    sendFriendRequestAgree(m_selectedRequestFromUser);
}

void MainWindow::onBtnRefuseClicked()
{
    sendFriendRequestRefuse(m_selectedRequestFromUser);
}

void MainWindow::sendFriendRequestAgree(const QString &fromUser)
{
    QNetworkRequest req(QUrl("https://www.singchat.chat/friend/handle"));
    req.setSslConfiguration(m_sslConf);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QByteArray postData = "from=" + fromUser.toUtf8() + "&to=" + m_currentUser.toUtf8() + "&agree=true";
    QNetworkReply *reply = m_netManager->post(req, postData);
    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();
        // =============0105新增：更新UI：隐藏按钮，显示状态标签
        ui->btnAgree->hide();
        ui->btnRefuse->hide();
        ui->Friend_status->show();
        ui->Friend_status->setText("您已同意好友请求");
        loadFriendRequests();
        QJsonObject reqMsg;
        reqMsg["type"] = "get_friends";
        reqMsg["from"] = m_currentUser;
        sendWebSocketMessage(QJsonDocument(reqMsg).toJson(QJsonDocument::Compact));
    });
}

void MainWindow::sendFriendRequestRefuse(const QString &fromUser)
{
    QNetworkRequest req(QUrl("https://www.singchat.chat/friend/handle"));
    req.setSslConfiguration(m_sslConf);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QByteArray postData = "from=" + fromUser.toUtf8() + "&to=" + m_currentUser.toUtf8() + "&agree=false";
    QNetworkReply *reply = m_netManager->post(req, postData);
    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();
        // =============0105新增：更新UI：隐藏按钮，显示状态标签
        ui->btnAgree->hide();
        ui->btnRefuse->hide();
        ui->Friend_status->show();
        ui->Friend_status->setText("您已拒绝好友请求");
        loadFriendRequests();
    });
}


// ============================0110晚上新增：分片发送文件逻辑====================
void MainWindow::onSendButtonClicked()
{
    if (m_selectedFriend.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先选择好友！");
        return;
    }

    // ===== 新增：优先处理文件消息（替换为统一分片传输） =====
    bool isFileMessage = !m_filePreviewMap.isEmpty();
    if (isFileMessage) {
        // 1. 安全获取最后一个文件信息
        FileSendInfo fileInfo;
        if (!m_filePreviewMap.isEmpty()) {
            fileInfo = (--m_filePreviewMap.end()).value();

            // 核心字段非空校验
            if (fileInfo.originalName.isEmpty() ||
                fileInfo.originalPath.isEmpty() ||
                fileInfo.fileType.isEmpty() ||
                fileInfo.tempPreviewPath.isEmpty()) {
                m_filePreviewMap.clear();
                qWarning() << "文件核心信息为空，无法发送给后端！";
                QMessageBox::warning(this, "提示", "文件信息无效，无法发送！");
                return;
            }

            // 本地文件存在性+可读性校验
            QFileInfo localFileInfo(fileInfo.tempPreviewPath);
            if (!localFileInfo.exists() || !localFileInfo.isFile() || !localFileInfo.isReadable()) {
                m_filePreviewMap.clear();
                qWarning() << "本地文件无效或不可读：" << fileInfo.tempPreviewPath;
                QMessageBox::warning(this, "提示", "本地文件已被删除或不可访问，无法发送！");
                return;
            }

            // 【0110晚上新增】核心修改：调用统一分片传输函数，替换原有一次性readAll()
            qDebug() << "【0110晚上新增】开始统一分片传输文件：" << fileInfo.originalName;
            // 【0112晚上修改：额外把文件类型也传过去】
            sendFileByChunk(fileInfo.tempPreviewPath, fileInfo.uniqueId, fileInfo.fileType);
            // 【0117晚上文件异步发送修改开始】
            // 初始化发送进度条（参考下载进度条逻辑）
            if (!m_isProgressBarInited) {
                ui->fileprogress->reset();
                ui->fileprogress->show();
                ui->fileprogress->setRange(0, 100);
                ui->fileprogress->setFormat("文件发送进度%p%");
                m_isProgressBarInited = true;
            }
            // 【0117晚上文件异步发送修改结束】
            // 7. 发送后清空缓存（避免重复发送）
            m_filePreviewMap.clear();
            ui->chat_input->clear();
            return;
        } else {
            return;
        }
    }

    // ===== 原有逻辑：处理纯文本消息（完整保留，无修改） =====
    QString content = ui->chat_input->toPlainText();
    if (content.isEmpty()) return;

    QJsonObject chatMsg;
    chatMsg["type"] = "chat";
    chatMsg["from"] = m_currentUser;
    chatMsg["to"] = m_selectedFriend;
    chatMsg["content"] = content;
    chatMsg["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    chatMsg["msg_content_type"] = "text"; // 文本消息类型
    // 【0118批注：这下面这条其实一点用都没，后端的消息结构体根本没有"global_msg_id"这个json字段】
    chatMsg["global_msg_id"] = QUuid::createUuid().toString(); // 补充全局消息ID，统一格式

    sendWebSocketMessage(QJsonDocument(chatMsg).toJson(QJsonDocument::Compact));
    addMessageToChatShow(m_currentUser, content, chatMsg["time"].toString());

    // ===== 清空输入框（原有逻辑，无修改） =====
    ui->chat_input->clear();
}

// ==============0106新增：对话消息框消息添加函数=====================
void MainWindow::addMessageToChatShow(const QString &sender, const QString &content, const QString &time)
{
    ChatMessage msg;
    msg.type = ChatMessage::Text;
    msg.owner = (sender == m_currentUser) ? ChatMessage::Self : ChatMessage::Other;
    msg.content = content;
    msg.senderName = sender;
    msg.receiverName = m_selectedFriend; // 设置接收者名称，用于本地缓存
    msg.timestamp = QDateTime::fromString(time, "yyyy-MM-dd hh:mm:ss");
    msg.isRead = true; // 发送的消息默认已读

    // 根据发送者选择头像 URL
    if (sender == m_currentUser) {
        msg.avatarPath = m_myAvatarUrl; // 自己的头像
    } else {
        // 从缓存里取好友头像，如果没有就用默认
        if (m_friendAvatars.contains(sender)) {
            msg.avatarPath = m_friendAvatars[sender];
        } else {
            msg.avatarPath = ":/avatars/other.png"; // 默认头像
        }
    }

    // 保存到本地缓存
    LocalChatCache::instance()->saveMessage(msg);
    
    m_chatModel->addMessage(msg);

    // 自动滚动到底部
    QMetaObject::invokeMethod(this, [=]() {
        // 使用更可靠的滚动方式
        QScrollBar *scrollBar = ui->chat_info_show->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());
    }, Qt::QueuedConnection);
}





// 【0112新增】完整无省略的排查函数
// 【修改】帧解析函数：支持提取分片payload+返回是否为最后一帧（保留所有调试日志）
QString MainWindow::parseWebSocketFrame(const QByteArray &frame, bool &isFinalFrame, quint8 &opcode, QByteArray &framePayload)
{
    // 初始化输出参数
    isFinalFrame = false;
    opcode = 0x00; // 【0112下午新增】：初始化 opcode 输出参数
    framePayload.clear();

    // ======================================
    // 步骤1：打印帧的基础信息（原始数据）
    // ======================================
    qDebug() << "======================================";
    qDebug() << "开始解析WebSocket帧";
    qDebug() << "原始帧总长度：" << frame.size() << "字节";
    if (frame.size() > 0) {
        qDebug() << "原始帧第一字节（十进制）：" << static_cast<quint8>(frame.at(0));
        qDebug() << "原始帧第一字节（十六进制）：" << QString::number(static_cast<quint8>(frame.at(0)), 16).toUpper();
    }
    if (frame.size() > 1) {
        qDebug() << "原始帧第二字节（十进制）：" << static_cast<quint8>(frame.at(1));
        qDebug() << "原始帧第二字节（十六进制）：" << QString::number(static_cast<quint8>(frame.at(1)), 16).toUpper();
    }
    qDebug() << "--------------------------------------";

    // ======================================
    // 步骤2：校验帧最小长度（至少2字节）
    // ======================================
    if (frame.size() < 2) {
        qDebug() << "解析失败：帧长度不足2字节，当前长度：" << frame.size();
        qDebug() << "======================================";
        return "";
    }

    // ======================================
    // 步骤3：解析FIN位（是否为最后一帧）+ opcode（操作码）
    // ======================================
    quint8 firstByte = static_cast<quint8>(frame.at(0));
    isFinalFrame = (firstByte & 0x80) != 0; // 提取最高位作为FIN标记（核心：判断是否为最后一帧）
    opcode = firstByte & 0x0F; // 【0112下午新增】：解析 opcode 并赋值给输出参数
    qDebug() << "解析opcode：" << QString::number(opcode, 16).toUpper() << "（十进制：" << opcode << "）";
    qDebug() << "解析FIN位：" << (isFinalFrame ? "是（最后一帧）" : "否（后续还有分片）");
    qDebug() << "opcode含义：" << (opcode == 0x00 ? "延续帧" :
                                       (opcode == 0x01 ? "文本帧" :
                                            (opcode == 0x02 ? "二进制帧" :
                                                 (opcode == 0x08 ? "关闭帧" :
                                                      (opcode == 0x09 ? "Ping帧" :
                                                           (opcode == 0x0A ? "Pong帧" : "未知帧"))))));

    // 支持文本帧（0x01）和延续帧（0x00），其他opcode直接返回空
    if (opcode != 0x01 && opcode != 0x00) {
        qDebug() << "解析失败：opcode不是文本帧（0x01）或延续帧（0x00），当前opcode：" << QString::number(opcode, 16).toUpper();
        qDebug() << "======================================";
        return "";
    }

    // ======================================
    // 步骤4：解析掩码位和初始payload长度
    // ======================================
    quint8 secondByte = static_cast<quint8>(frame.at(1));
    bool hasMask = (secondByte & 0x80) != 0; // 提取最高位作为掩码标识
    quint64 payloadLen = secondByte & 0x7F;  // 提取低7位作为初始payload长度
    qDebug() << "--------------------------------------";
    qDebug() << "是否包含掩码：" << (hasMask ? "是" : "否");
    qDebug() << "初始payload长度（低7位）：" << payloadLen << "（十六进制：" << QString::number(payloadLen, 16).toUpper() << "）";

    // ======================================
    // 步骤5：解析完整payload长度（处理126、127）
    // ======================================
    int offset = 2; // 初始偏移量（前2字节已解析）
    if (payloadLen == 126) {
        qDebug() << "处理payload长度：126，需要后续2字节解析完整长度";
        // 校验帧长度是否足够（至少包含后续2字节）
        if (frame.size() < offset + 2) {
            qDebug() << "解析失败：payloadLen=126，但帧长度不足，无法读取后续2字节，当前帧长度：" << frame.size() << "，需要至少：" << offset + 2;
            qDebug() << "======================================";
            return "";
        }
        // 解析2字节无符号整数（大端序）
        quint16 payloadLen16 = (static_cast<quint16>(static_cast<quint8>(frame.at(offset))) << 8) |
                               static_cast<quint16>(static_cast<quint8>(frame.at(offset + 1)));
        payloadLen = static_cast<quint64>(payloadLen16);
        offset += 2; // 偏移量增加2字节
        qDebug() << "解析到完整payload长度（126对应）：" << payloadLen << "字节（十六进制：" << QString::number(payloadLen, 16).toUpper() << "）";
    } else if (payloadLen == 127) {
        qDebug() << "处理payload长度：127，需要后续8字节解析完整长度";
        // 校验帧长度是否足够（至少包含后续8字节）
        if (frame.size() < offset + 8) {
            qDebug() << "解析失败：payloadLen=127，但帧长度不足，无法读取后续8字节，当前帧长度：" << frame.size() << "，需要至少：" << offset + 8;
            qDebug() << "======================================";
            return "";
        }
        // 解析8字节无符号整数（大端序，简化处理：支持最大64位无符号整数）
        quint64 payloadLen64 = 0;
        for (int i = 0; i < 8; ++i) {
            payloadLen64 = (payloadLen64 << 8) | static_cast<quint64>(static_cast<quint8>(frame.at(offset + i)));
        }
        payloadLen = payloadLen64;
        offset += 8; // 偏移量增加8字节
        qDebug() << "解析到完整payload长度（127对应）：" << payloadLen << "字节（十六进制：" << QString::number(payloadLen, 16).toUpper() << "）";
    } else {
        qDebug() << "payload长度无需扩展，直接使用：" << payloadLen << "字节";
    }

    // ======================================
    // 步骤6：解析掩码键（如果有掩码）
    // ======================================
    QByteArray maskingKey;
    if (hasMask) {
        qDebug() << "--------------------------------------";
        qDebug() << "开始解析掩码键（4字节）";
        // 校验帧长度是否足够（至少包含4字节掩码键）
        if (frame.size() < offset + 4) {
            qDebug() << "解析失败：存在掩码，但帧长度不足，无法读取4字节掩码键，当前帧长度：" << frame.size() << "，需要至少：" << offset + 4;
            qDebug() << "======================================";
            return "";
        }
        // 提取4字节掩码键
        maskingKey = frame.mid(offset, 4);
        offset += 4; // 偏移量增加4字节
        // 打印掩码键详情
        qDebug() << "掩码键长度：" << maskingKey.size() << "字节";
        qDebug() << "掩码键内容（十六进制）：" << maskingKey.toHex().toUpper();
        qDebug() << "掩码键内容（十进制）：" << QString::number(static_cast<quint8>(maskingKey.at(0))) << ","
                 << QString::number(static_cast<quint8>(maskingKey.at(1))) << ","
                 << QString::number(static_cast<quint8>(maskingKey.at(2))) << ","
                 << QString::number(static_cast<quint8>(maskingKey.at(3)));
    }

    // ======================================
    // 步骤7：提取payload数据（消息正文，核心：返回payload给分片缓存）
    // ======================================
    qDebug() << "--------------------------------------";
    qDebug() << "开始提取payload数据";
    qDebug() << "当前偏移量：" << offset << "字节";
    qDebug() << "预期payload长度：" << payloadLen << "字节";
    qDebug() << "剩余帧数据长度：" << (frame.size() - offset) << "字节";

    // 校验帧长度是否足够容纳payload
    if (static_cast<quint64>(frame.size() - offset) < payloadLen) {
        qDebug() << "解析失败：帧剩余数据不足，无法提取完整payload，剩余数据：" << (frame.size() - offset) << "字节，需要：" << payloadLen << "字节";
        qDebug() << "======================================";
        return "";
    }

    // 提取payload数据（核心：赋值给输出参数framePayload）
    framePayload = frame.mid(offset, static_cast<int>(payloadLen));
    qDebug() << "实际提取payload长度：" << framePayload.size() << "字节";
    qDebug() << "payload前100字节（原始十六进制）：" << framePayload.left(100).toHex().toUpper();
    qDebug() << "payload前100字节（原始ASCII）：" << QString::fromLatin1(framePayload.left(100));

    // ======================================
    // 步骤8：掩码解密（如果有掩码）
    // ======================================
    if (hasMask && !maskingKey.isEmpty()) {
        qDebug() << "--------------------------------------";
        qDebug() << "开始进行掩码异或解密";
        // 循环异或解密（payload[i] ^= maskingKey[i % 4]）
        for (int i = 0; i < framePayload.size(); ++i) {
            framePayload[i] = static_cast<char>(static_cast<quint8>(framePayload[i]) ^ static_cast<quint8>(maskingKey.at(i % 4)));
        }
        qDebug() << "解密完成，payload前100字节（解密后十六进制）：" << framePayload.left(100).toHex().toUpper();
        qDebug() << "解密完成，payload前100字节（解密后ASCII）：" << QString::fromLatin1(framePayload.left(100));
    }

    // ======================================
    // 步骤9：转换为QString（UTF-8编码）并返回
    // ======================================
    qDebug() << "--------------------------------------";
    qDebug() << "开始转换payload为UTF-8编码QString";
    QString result = QString::fromUtf8(framePayload);
    qDebug() << "转换结果：" << (result.isEmpty() ? "空字符串" : "非空字符串");
    qDebug() << "转换后字符串长度：" << result.size() << "字符";
    qDebug() << "转换后字符串前100字符：" << result.left(100);
    qDebug() << "解析流程结束";
    qDebug() << "======================================";

    return result;
}

// 【0112新增】辅助函数：计算完整WebSocket帧长度（核心，判断是否有完整帧，解决截断）
// 作用：判断缓存中是否有完整帧，解决消息截断问题
// 【修改】辅助函数：计算完整WebSocket帧长度（支持延续帧opcode=0x00，解决分片问题）
quint64 MainWindow::calculateCompleteWebSocketFrameLength(const QByteArray &cache)
{
    // 步骤1：校验最小帧长度（至少2字节）
    if (cache.size() < 2) {
        return 0;
    }

    // 步骤2：解析opcode（支持文本帧0x01 + 延续帧0x00，核心修改）
    quint8 firstByte = static_cast<quint8>(cache.at(0));
    quint8 opcode = firstByte & 0x0F;
    if (opcode != 0x01 && opcode != 0x00) { // 只处理文本帧和延续帧，其他帧忽略
        return 0;
    }

    // 步骤3：解析掩码位和初始payload长度（原有逻辑，保留）
    quint8 secondByte = static_cast<quint8>(cache.at(1));
    bool hasMask = (secondByte & 0x80) != 0;
    quint64 payloadLen = secondByte & 0x7F;
    int offset = 2; // 初始偏移量（前2字节）

    // 步骤4：解析完整payload长度（处理126/127，原有逻辑，保留）
    if (payloadLen == 126) {
        if (cache.size() < offset + 2) {
            return 0; // 长度不足，无法解析
        }
        quint16 payloadLen16 = (static_cast<quint16>(static_cast<quint8>(cache.at(offset))) << 8) |
                               static_cast<quint16>(static_cast<quint8>(cache.at(offset + 1)));
        payloadLen = static_cast<quint64>(payloadLen16);
        offset += 2;
    } else if (payloadLen == 127) {
        if (cache.size() < offset + 8) {
            return 0; // 长度不足，无法解析
        }
        quint64 payloadLen64 = 0;
        for (int i = 0; i < 8; ++i) {
            payloadLen64 = (payloadLen64 << 8) | static_cast<quint64>(static_cast<quint8>(cache.at(offset + i)));
        }
        payloadLen = payloadLen64;
        offset += 8;
    }

    // 步骤5：解析掩码键长度（原有逻辑，保留）
    int maskingKeyLen = hasMask ? 4 : 0;
    if (hasMask && cache.size() < offset + 4) {
        return 0; // 长度不足，无法读取掩码键
    }
    offset += maskingKeyLen;

    // 步骤6：计算完整帧总长度（帧头+掩码键+payload）
    quint64 completeFrameLen = offset + payloadLen;

    // 步骤7：二次校验（缓存长度是否足够容纳完整帧）
    if (completeFrameLen > static_cast<quint64>(cache.size())) {
        return 0;
    }

    return completeFrameLen;
}

void MainWindow::onSslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    QList<QSslError> ignore;
    for (const QSslError &err : errors) {
        if (err.error() == QSslError::HostNameMismatch ||
            err.error() == QSslError::SelfSignedCertificate ||
            err.error() == QSslError::CertificateUntrusted) {
            ignore << err;
        }
    }
    if (!ignore.isEmpty()) {
        reply->ignoreSslErrors(ignore);
    }
}

void MainWindow::toggleMaximize()
{
    if (isMaximized()) {
        showNormal(); // 还原窗口
    } else {
        showMaximized(); // 最大化窗口
    }
}



// 判断鼠标是否在标题栏区域（顶部30px）
bool MainWindow::isInTitleBar(const QPoint &pos)
{
    return pos.y() < 30;
}

// 判断鼠标是否在窗口边缘
int MainWindow::getResizeEdge(const QPoint &pos)
{
    int borderWidth = 5;
    int edge = 0;

    if (pos.x() < borderWidth) edge |= 1; // 左边缘
    if (pos.x() > width() - borderWidth) edge |= 2; // 右边缘
    if (pos.y() < borderWidth) edge |= 4; // 上边缘
    if (pos.y() > height() - borderWidth) edge |= 8; // 下边缘

    return edge;
}

// 更新光标形状
void MainWindow::updateCursorShape(const QPoint &pos)
{
    if (isResizing) {
        if (resizeEdge == (4 | 1) || resizeEdge == (8 | 2)) {
            setCursor(Qt::SizeFDiagCursor); // 对角线光标（左上-右下）
        } else if (resizeEdge == (4 | 2) || resizeEdge == (8 | 1)) {
            setCursor(Qt::SizeBDiagCursor); // 对角线光标（右上-左下）
        } else if (resizeEdge & 1 || resizeEdge & 2) {
            setCursor(Qt::SizeHorCursor); // 水平光标
        } else if (resizeEdge & 4 || resizeEdge & 8) {
            setCursor(Qt::SizeVerCursor); // 垂直光标
        } else {
            setCursor(Qt::ArrowCursor); // 默认光标
        }
        return;
    }

    int edge = getResizeEdge(pos);
    if (edge == (4 | 1) || edge == (8 | 2)) {
        setCursor(Qt::SizeFDiagCursor);
    } else if (edge == (4 | 2) || edge == (8 | 1)) {
        setCursor(Qt::SizeBDiagCursor);
    } else if (edge & 1 || edge & 2) {
        setCursor(Qt::SizeHorCursor);
    } else if (edge & 4 || edge & 8) {
        setCursor(Qt::SizeVerCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
}

// 处理缩放逻辑
void MainWindow::handleResize(QMouseEvent *event)
{
    QRect newGeometry = resizeStartGeometry;
    QPoint delta = event->globalPosition().toPoint() - resizeStartPosition;

    if (resizeEdge & 1) { // 左边缘
        newGeometry.setLeft(resizeStartGeometry.left() + delta.x());
        if (newGeometry.width() < minimumWidth()) {
            newGeometry.setLeft(resizeStartGeometry.right() - minimumWidth());
        }
    }
    if (resizeEdge & 2) { // 右边缘
        newGeometry.setRight(resizeStartGeometry.right() + delta.x());
        if (newGeometry.width() < minimumWidth()) {
            newGeometry.setRight(resizeStartGeometry.left() + minimumWidth());
        }
    }
    if (resizeEdge & 4) { // 上边缘
        newGeometry.setTop(resizeStartGeometry.top() + delta.y());
        if (newGeometry.height() < minimumHeight()) {
            newGeometry.setTop(resizeStartGeometry.bottom() - minimumHeight());
        }
    }
    if (resizeEdge & 8) { // 下边缘
        newGeometry.setBottom(resizeStartGeometry.bottom() + delta.y());
        if (newGeometry.height() < minimumHeight()) {
            newGeometry.setBottom(resizeStartGeometry.top() + minimumHeight());
        }
    }

    setGeometry(newGeometry);
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        int edge = getResizeEdge(event->pos());
        if (edge != 0) {
            isResizing = true;
            resizeEdge = edge;
            resizeStartPosition = event->globalPosition().toPoint();
            resizeStartGeometry = geometry();
            event->accept();
            return;
        }

        if (isInTitleBar(event->pos()) &&
            !ui->close_botton->geometry().contains(event->pos()) &&
            !ui->maximize_botton->geometry().contains(event->pos()) &&
            !ui->minimize_botton->geometry().contains(event->pos())) {

            QPoint mouseDownPos = event->globalPosition().toPoint();

            if (isMaximized()) {
                // 先还原窗口
                showNormal();

                // 让窗口的顶部中点在鼠标位置偏下 20px
                QPoint newWindowPos = mouseDownPos - QPoint(width() / 2, 20);
                move(newWindowPos);

                // 更新拖动偏移量
                dragStartPosition = mouseDownPos - frameGeometry().topLeft();
            } else {
                dragStartPosition = mouseDownPos - frameGeometry().topLeft();
            }

            isDragging = true;
            event->accept();
            return;
        }
    }
    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (isDragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPosition().toPoint() - dragStartPosition);
        event->accept();
        return;
    }

    if (isResizing && (event->buttons() & Qt::LeftButton)) {
        handleResize(event);
        event->accept();
        return;
    }

    updateCursorShape(event->pos());
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
        isResizing = false;
        resizeEdge = 0;
        updateCursorShape(event->pos());
        event->accept();
        return;
    }
    QMainWindow::mouseReleaseEvent(event);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // ========== 通用处理：所有QListView的丝滑滚轮事件 ==========
    QListView *targetList = qobject_cast<QListView*>(obj);
    // 【0118批注：之后如果还有控件要实现滚动条，直接在这里多加一个obj的名称】
    if (targetList && (obj == ui->chat_info_show || obj == ui->person_list || obj == ui->friendRequest_list || obj == ui->group_list)) {
        // 1. 处理滚轮事件（丝滑滚动，5像素/格）
        if (event->type() == QEvent::Wheel) {
            QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
            wheelEvent->accept();
            int scrollStep = 5;
            int delta = (wheelEvent->angleDelta().y() > 0) ? -scrollStep : scrollStep;
            QScrollBar *vScrollBar = targetList->verticalScrollBar();
            if (vScrollBar) {
                vScrollBar->setValue(vScrollBar->value() + delta);
            }
            return true;
        }

        // 2. 处理鼠标进入/离开（滚动条显隐）
        QScrollBar *vScrollBar = targetList->verticalScrollBar();
        if (vScrollBar) {
            bool hasScrollableContent = (vScrollBar->maximum() > 0);
            if (event->type() == QEvent::Enter) {
                showListViewScrollBar(targetList, hasScrollableContent);
                return true;
            } else if (event->type() == QEvent::Leave) {
                showListViewScrollBar(targetList, false);
                return true;
            }
        }
    }
    // ===== 原有逻辑：鼠标移动更新光标形状 =====
    if (event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (obj == this || isAncestorOf(qobject_cast<QWidget*>(obj))) {
            QPoint globalPos = mouseEvent->globalPosition().toPoint();
            if (geometry().contains(globalPos)) {
                QPoint localPos = mapFromGlobal(globalPos);
                updateCursorShape(localPos);
            }
        }
    }

    // ===== 0105新增：输入框按回车键发送消息 =====
    else if (obj == ui->chat_input && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        // 检测是否按下了回车键（不含Shift）
        if (keyEvent->key() == Qt::Key_Return && !(keyEvent->modifiers() & Qt::ShiftModifier)) {
            onSendButtonClicked(); // 调用发送消息
            return true; // 阻止默认的换行行为
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

QList<QSslError> MainWindow::ignoreCriticalSslErrors(const QList<QSslError> &errors, QString &errorSummary)
{
    qDebug() << "📌 SSL错误列表：";
    QList<QSslError> ignoreErrors;
    errorSummary.clear();

    for (const QSslError &err : errors) {
        QString errStr = err.errorString();
        qDebug() << "  - " << errStr;
        errorSummary += errStr + "\n";

        // 忽略常见的证书错误
        if (err.error() == QSslError::HostNameMismatch) {
            ignoreErrors.append(err);
        } else if (errStr.contains("The issuer certificate of a locally looked up certificate could not be found")) {
            ignoreErrors.append(err);
        }
    }

    return ignoreErrors;
}

// ========== 通用版：控制任意QListView的滚动条显隐（替换原有showChatScrollBar） ==========
void MainWindow::showListViewScrollBar(QListView *listView, bool show)
{
    if (!listView) return; // 判空，避免崩溃
    
    // 保存当前滚动位置，防止样式修改导致滚动跳动
    QScrollBar *scrollBar = listView->verticalScrollBar();
    int scrollPos = scrollBar ? scrollBar->value() : 0;

    // 基础样式（保留核心背景/横向滚动条隐藏）
    QString baseStyle = QString(R"(
        /* 为%1设置整体背景色（复用原有#F0F0F0，可根据需求修改） */
        QListView#%1 {
            background-color: #F0F0F0;
            border: none;
            qproperty-horizontalScrollBarPolicy: ScrollBarAlwaysOff;
        }

        /* 设置视口背景，确保底色完全覆盖 */
        QListView#%1 QAbstractItemView::viewport {
            background-color: #F0F0F0;
        }

        /* 彻底隐藏横向滚动条 */
        QListView#%1 QScrollBar:horizontal {
            height: 0px;
            background: transparent;
            margin: 0px;
            padding: 0px;
        }

        QListView#%1 QScrollBar::groove:horizontal,
        QListView#%1 QScrollBar::handle:horizontal,
        QListView#%1 QScrollBar::sub-line:horizontal,
        QListView#%1 QScrollBar::add-line:horizontal {
            height: 0px;
            width: 0px;
            background: transparent;
            margin: 0px;
            padding: 0px;
        }

        QListView#%1 QScrollBar::left-arrow:horizontal,
        QListView#%1 QScrollBar::right-arrow:horizontal {
            image: none;
        }
    )").arg(listView->objectName()); // 动态替换列表名称

    // 垂直滚动条样式（7px宽+3px圆角+全0边距）
    QString scrollBarStyle;
    if (show) {
        // 显示状态：严格全0边距+兼容型圆角
        scrollBarStyle = QString(R"(
            /* 滚动条整体：7px宽+全0边距 */
            QListView#%1 QScrollBar:vertical {
                width: 7px;
                background: transparent;
                margin: 0px !important;  /* 强制全0 */
                padding: 0px !important; /* 强制全0 */
            }

            /* 滑槽：全0边距+3px圆角 */
            QListView#%1 QScrollBar::groove:vertical {
                background: transparent;
                border-radius: 3px;
                margin: 0px !important;
                padding: 0px !important;
            }

            /* 滑块：核心！全0边距+3px圆角+无挤压 */
            QListView#%1 QScrollBar::handle:vertical {
                background: #C8C8C8;
                border-radius: 3px;      /* 7px宽用3px圆角，Qt渲染最兼容 */
                margin: 0px !important;  /* 彻底取消所有边距，避免挤压 */
                padding: 0px !important;
                min-height: 30px;        /* 最小高度足够，保证圆角显示 */
            }

            /* 滑块hover状态 */
            QListView#%1 QScrollBar::handle:vertical:hover {
                background: #A8A8A8;
            }

            /* 滑块按下状态 */
            QListView#%1 QScrollBar::handle:vertical:pressed {
                background: #888888;
            }

            /* 隐藏箭头：全0边距 */
            QListView#%1 QScrollBar::sub-line:vertical,
            QListView#%1 QScrollBar::add-line:vertical {
                height: 0px;
                width: 0px;
                margin: 0px;
                padding: 0px;
            }

            QListView#%1 QScrollBar::up-arrow:vertical,
            QListView#%1 QScrollBar::down-arrow:vertical {
                image: none;
            }

            QListView#%1 QScrollBar::corner:vertical {
                background: transparent;
                margin: 0px;
                padding: 0px;
            }
        )").arg(listView->objectName());
    } else {
        // 隐藏状态：仅滑块透明，其余和显示状态完全一致
        scrollBarStyle = QString(R"(
            QListView#%1 QScrollBar:vertical {
                width: 7px;
                background: transparent;
                margin: 0px !important;
                padding: 0px !important;
            }

            QListView#%1 QScrollBar::groove:vertical {
                background: transparent;
                border-radius: 3px;
                margin: 0px !important;
                padding: 0px !important;
            }

            QListView#%1 QScrollBar::handle:vertical {
                background: transparent; /* 仅透明，其余不变 */
                border-radius: 3px;
                margin: 0px !important;
                padding: 0px !important;
                min-height: 30px;
            }

            QListView#%1 QScrollBar::handle:vertical:hover,
            QListView#%1 QScrollBar::handle:vertical:pressed {
                background: transparent;
            }

            QListView#%1 QScrollBar::sub-line:vertical,
            QListView#%1 QScrollBar::add-line:vertical {
                height: 0px;
                width: 0px;
                margin: 0px;
                padding: 0px;
            }

            QListView#%1 QScrollBar::up-arrow:vertical,
            QListView#%1 QScrollBar::down-arrow:vertical {
                image: none;
            }

            QListView#%1 QScrollBar::corner:vertical {
                background: transparent;
                margin: 0px;
                padding: 0px;
            }
        )").arg(listView->objectName());
    }

    // 合并样式并应用
    listView->setStyleSheet(baseStyle + scrollBarStyle);
    
    // 恢复之前保存的滚动位置
    if (scrollBar) {
        scrollBar->setValue(scrollPos);
    }
}


// ================================0110文件传输相关===============================
// ---------------------- 2. 实现ChatInputTextEdit的辅助函数（判断文件格式是否支持） ----------------------
bool ChatInputTextEdit::isSupportFileFormat(const QString &filePath)
{
    // 提取文件后缀（转为小写，避免大小写问题，如.PNG和.png都支持）
    QFileInfo fileInfo(filePath);
    QString fileSuffix = fileInfo.suffix().toLower();
    QString fullSuffix = QString(".%1").arg(fileSuffix);

    // 匹配支持的格式集合（这里直接引用MainWindow的静态常量）
    return MainWindow::SUPPORT_IMAGE_FORMATS.contains(fullSuffix) ||
           MainWindow::SUPPORT_VIDEO_FORMATS.contains(fullSuffix) ||
           MainWindow::SUPPORT_COMPRESS_FORMATS.contains(fullSuffix);
}

// ---------------------- 3. 实现ChatInputTextEdit的dragEnterEvent（拖拽进入） ----------------------
void ChatInputTextEdit::dragEnterEvent(QDragEnterEvent *event)
{
    // 1. 判断拖拽数据是否包含本地文件
    if (!event->mimeData()->hasUrls() || event->mimeData()->urls().isEmpty()) {
        event->ignore();
        return;
    }

    // 2. 提取第一个文件路径（仅支持单个文件拖拽）
    QUrl fileUrl = event->mimeData()->urls().first();
    QString filePath = fileUrl.toLocalFile();

    // 3. 判断文件格式是否支持
    if (isSupportFileFormat(filePath)) {
        // 【修改】视觉反馈：仅设置深灰色背景（#E0E0E0，比#F0F0F0深），移除虚线边框
        this->setStyleSheet(m_originalStyleSheet + "#chat_input { background-color: #E0E0E0 !important; }");

        // 5. 接受拖拽事件，允许后续放下操作
        event->acceptProposedAction();
    } else {
        // 不支持的格式，拒绝拖拽
        event->ignore();
    }
}

// ---------------------- 4. 实现ChatInputTextEdit的dragMoveEvent（拖拽移动） ----------------------
void ChatInputTextEdit::dragMoveEvent(QDragMoveEvent *event)
{
    // 逻辑与dragEnterEvent一致，维持高亮边框，确保拖拽不失效
    if (!event->mimeData()->hasUrls() || event->mimeData()->urls().isEmpty()) {
        event->ignore();
        return;
    }

    QUrl fileUrl = event->mimeData()->urls().first();
    QString filePath = fileUrl.toLocalFile();

    if (isSupportFileFormat(filePath)) {
        // 【修改】维持深灰色背景（#E0E0E0），移除虚线边框
        this->setStyleSheet(m_originalStyleSheet + "#chat_input { background-color: #E0E0E0 !important; }");
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

// ---------------------- 5. 实现ChatInputTextEdit的dropEvent（拖拽放下） ----------------------
void ChatInputTextEdit::dropEvent(QDropEvent *event)
{
    // 1. 先恢复输入框默认样式（取消高亮边框）
    // 【修改】直接还原chat_input的原始样式表（无需手动写默认样式）
    this->setStyleSheet(m_originalStyleSheet);

    // 2. 判断拖拽数据是否包含本地文件
    if (!event->mimeData()->hasUrls() || event->mimeData()->urls().isEmpty()) {
        event->ignore();
        return;
    }

    // 3. 提取第一个文件路径（仅支持单个文件拖拽）
    QUrl fileUrl = event->mimeData()->urls().first();
    QString filePath = fileUrl.toLocalFile();

    // 4. 容错处理：判断文件是否存在且可读取
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isFile() || !fileInfo.isReadable()) {
        QMessageBox::warning(this, "文件错误", "该文件不存在或无法读取，请选择有效文件！");
        event->ignore();
        return;
    }

    // 5. 再次判断格式（双重校验，避免异常）
    if (isSupportFileFormat(filePath)) {
        // 6. 发送信号：将文件路径传递给MainWindow，处理后续预览生成
        emit fileDropped(filePath);
        event->acceptProposedAction();
    } else {
        QMessageBox::warning(this, "格式不支持", "该文件格式不支持，请选择图片、视频或压缩文件！");
        event->ignore();
    }
}

// ---------------------- 6. 新增：实现ChatInputTextEdit的dragLeaveEvent（拖拽离开） ----------------------
void ChatInputTextEdit::dragLeaveEvent(QDragLeaveEvent *event)
{
    // 核心逻辑：拖拽离开控件区域，直接还原原始样式表
    this->setStyleSheet(m_originalStyleSheet);
    event->accept();
}

// ---------------------- 7. 新增：辅助函数：从剪贴板提取图片文件路径（ChatInputTextEdit内部使用） ----------------------
QString ChatInputTextEdit::getImagePathFromClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();

    // 1. 优先判断剪贴板是否包含本地文件（比如复制文件夹中的图片文件后粘贴）
    if (mimeData->hasUrls() && !mimeData->urls().isEmpty()) {
        QList<QUrl> urlList = mimeData->urls(); // 保存临时对象为局部变量，延长生命周期
        QUrl fileUrl;
        if (!urlList.isEmpty()) { // 先判断列表非空，避免空列表调用first()导致崩溃
            fileUrl = urlList.first(); // 对局部变量调用first()，无临时对象操作，消除警告
        } else {
            qWarning() << "⚠️ 未获取到有效URL列表";
            return ""; // 或执行其他错误处理逻辑
        }
        QString filePath = fileUrl.toLocalFile();

        // 2. 仅返回支持的图片格式文件
        QFileInfo fileInfo(filePath);
        QString fileSuffix = QString(".%1").arg(fileInfo.suffix().toLower());
        if (MainWindow::SUPPORT_IMAGE_FORMATS.contains(fileSuffix) && fileInfo.exists() && fileInfo.isReadable()) {
            return filePath;
        }
    }

    // 3. 补充：若剪贴板包含图片数据（比如截图，无本地文件路径），此处可后续扩展
    // （当前先实现本地图片文件粘贴，截图粘贴需额外处理图片保存，暂返回空字符串）
    return "";
}

// ---------------------- 8. 新增：实现ChatInputTextEdit的keyPressEvent（监听Ctrl+V） ----------------------
void ChatInputTextEdit::keyPressEvent(QKeyEvent *event)
{
    // 1. 监听Ctrl+V快捷键（粘贴操作）
    if (event->key() == Qt::Key_V && event->modifiers() == Qt::ControlModifier) {
        // 2. 从剪贴板提取图片文件路径
        QString imageFilePath = getImagePathFromClipboard();
        if (!imageFilePath.isEmpty()) {
            // 3. 调用现有逻辑：发送fileDropped信号，复用后续预览生成流程
            emit fileDropped(imageFilePath);
            event->accept();
            return;
        }
    }

    // 4. 非Ctrl+V操作，执行QTextEdit的默认键盘逻辑（保证正常输入/删除等功能）
    QTextEdit::keyPressEvent(event);
}

// =================================如上为0110文件传输相关======================

// ===========================如下为0110文件传输展示逻辑===========================
// ---------------------- 辅助函数：格式化文件大小 ----------------------
QString MainWindow::formatFileSize(qint64 fileSizeBytes)
{
    if (fileSizeBytes < 0) return "未知大小";
    if (fileSizeBytes == 0) return "0 B";

    // 定义单位数组
    QStringList units = {"B", "KB", "MB", "GB"};
    int unitIndex = 0;
    double fileSize = fileSizeBytes;

    // 转换单位（1024进制）
    while (fileSize >= 1024 && unitIndex < units.size() - 1) {
        fileSize /= 1024;
        unitIndex++;
    }

    // 保留1位小数，拼接单位返回
    return QString("%1 %2").arg(fileSize, 0, 'f', 1).arg(units[unitIndex]);
}

// ---------------------- 辅助函数：格式化过长文件名 ----------------------
QString MainWindow::formatFileName(const QString &originalName)
{
    const int maxLength = 20;
    if (originalName.length() <= maxLength) return originalName;

    // 首尾各取8位，中间加....
    QString prefix = originalName.left(8);
    QString suffix = originalName.right(8);
    return QString("%1....%2").arg(prefix,suffix);
}

// ---------------------- 辅助函数：插入图片预览 ----------------------
void MainWindow::insertImagePreview(const QString &filePath, const QString &fileName)
{
    QTextEdit *chatInput = ui->chat_input;
    QTextCursor cursor = chatInput->textCursor();

    // 1. 加载图片并生成缩略图（限制最大宽高150px，保持比例）
    QImage image(filePath);
    if (image.isNull()) {
        // 图片加载失败，转为文件预览
        insertFilePreview(fileName, formatFileSize(QFileInfo(filePath).size()), "image");
        return;
    }
    QImage thumbnail = image.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // 2. 插入图片到QTextEdit（富文本格式）
    QTextImageFormat imageFormat;
    // 临时保存缩略图到缓存（也可直接用原路径，此处为避免原图过大）
    QString tempImagePath = QString("%1/_temp_thumb_%2.png").arg(QDir::tempPath(), QUuid::createUuid().toString());
    thumbnail.save(tempImagePath);
    imageFormat.setName(tempImagePath);
    imageFormat.setWidth(thumbnail.width());
    imageFormat.setHeight(thumbnail.height());

    cursor.insertImage(imageFormat);
    // 【删除此行】去掉图片后的文件名文字，仅保留图片
    // cursor.insertText(QString("  %1\n").arg(fileName));

    // 3. 更新光标位置
    chatInput->setTextCursor(cursor);
}

// ---------------------- 辅助函数：插入视频/压缩文件预览（图标+文字合成单张图片） ----------------------
void MainWindow::insertFilePreview(const QString &fileName, const QString &fileSize, const QString &fileType)
{
    QTextEdit *chatInput = ui->chat_input;
    QTextCursor cursor = chatInput->textCursor();

    // 1. 加载本地图标图片
    QString appRunPath = QCoreApplication::applicationDirPath();
    QString localIconPath;
    if (fileType == "video") {
        localIconPath = QString("%1%2btnimage%2视频播放图.png")
                            .arg(appRunPath)
                            .arg(QDir::separator());
    } else {
        localIconPath = QString("%1%2btnimage%2压缩文件图.png")
                            .arg(appRunPath)
                            .arg(QDir::separator());
    }

    // 2. 加载图标（60x60）
    QPixmap iconPixmap(localIconPath);
    if (iconPixmap.isNull()) {
        qWarning() << "本地图标加载失败，使用兜底图标：" << localIconPath;
        iconPixmap = QApplication::style()->standardIcon(QStyle::SP_FileIcon).pixmap(70, 70);
    } else {
        iconPixmap = iconPixmap.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    // 3. 定义合成图片的尺寸（宽300，高80，足够容纳图标+文字，可自定义）
    QSize compositeSize(350, 70);
    QPixmap compositePixmap(compositeSize);
    compositePixmap.fill(Qt::transparent); // 填充透明背景，避免遮挡

    // 4. 用QPainter绘制合成图（图标+文字）
    QPainter painter(&compositePixmap);
    painter.setRenderHint(QPainter::Antialiasing, true); // 抗锯齿，提升美观度

    // 4.1 绘制图标（左对齐，上下居中，留10px边距）
    int iconX = 10;
    int iconY = (compositeSize.height() - iconPixmap.height()) / 2;
    painter.drawPixmap(iconX, iconY, iconPixmap);

    // 4.2 绘制文字（文件名+大小，跟在图标右侧，上下居中，留20px边距）
    QFont font;
    font.setPointSize(10); // 设置字体大小
    painter.setFont(font);
    painter.setPen(Qt::black); // 文字颜色

    QString fileInfoText = QString("%1 （%2）").arg(fileName, fileSize);
    int textX = iconX + iconPixmap.width() + 20; // 文字X坐标：图标右侧+20px边距
    // int textY = compositeSize.height() / 2;
    QRect textRect(textX, 0, compositeSize.width() - textX - 10, compositeSize.height());

    // 文字居中绘制（垂直居中）
    painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, fileInfoText);

    // 4.3 结束绘制
    painter.end();

    // 5. 插入合成图到QTextEdit（富文本格式）
    QTextImageFormat imageFormat;
    QString tempCompositePath = QString("%1/_temp_composite_%2.png")
                                    .arg(QDir::tempPath(), QUuid::createUuid().toString());
    compositePixmap.save(tempCompositePath); // 保存合成图到临时路径

    // 设置合成图显示尺寸（和画布尺寸一致，避免拉伸）
    imageFormat.setName(tempCompositePath);
    imageFormat.setWidth(compositeSize.width());
    imageFormat.setHeight(compositeSize.height());

    cursor.insertImage(imageFormat); // 仅插入合成图，无单独文字

    // 6. 更新光标位置
    chatInput->setTextCursor(cursor);
}




// ---------------------- 9. 实现MainWindow的onFileDropped槽函数 ----------------------
// ---------------------- 实现MainWindow的onFileDropped槽函数（完整版本） ----------------------
void MainWindow::onFileDropped(const QString &filePath)
{
    // 1. 基础校验：文件是否存在且可读取
    QFileInfo fileInfo(filePath);

    if (!fileInfo.exists() || !fileInfo.isFile() || !fileInfo.isReadable()) {
        qWarning() << "文件无效或无法读取：" << filePath;
        QMessageBox::warning(this, "提示", "文件无效或无法读取！");
        return;
    }

    qDebug() << "拖拽/粘贴成功，文件路径：" << filePath;

    // 2. 提取文件核心信息
    QString originalFileName = fileInfo.fileName();
    QString fileSuffix = QString(".%1").arg(fileInfo.suffix().toLower());
    qint64 fileSizeBytes = fileInfo.size();
    QString formattedFileName = formatFileName(originalFileName);
    QString formattedFileSize = formatFileSize(fileSizeBytes);

    // 3. 初始化FileSendInfo结构体，填充数据
    FileSendInfo fileSendInfo;
    // 【0113晚上关键修改】指定QUuid::WithoutBraces，生成不带大括号的UUID
    fileSendInfo.uniqueId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    qDebug() << "生成的不带大括号的唯一ID：" << fileSendInfo.uniqueId;
    fileSendInfo.originalPath = filePath;
    fileSendInfo.originalName = originalFileName;
    fileSendInfo.formatName = formattedFileName;
    fileSendInfo.formatSize = formattedFileSize;
    fileSendInfo.tempPreviewPath = ""; // 预览路径：图片填充缩略图路径，其他填充图标路径

    // 【新增】赋值文件后缀，传给后续发送逻辑
    fileSendInfo.fileSuffix = fileSuffix;

    // 4. 按文件类型分类处理（预览+填充fileType）
    if (SUPPORT_IMAGE_FORMATS.contains(fileSuffix)) {
        // 图片文件：插入缩略图预览
        fileSendInfo.fileType = "image";
        insertImagePreview(filePath, formattedFileName);
        fileSendInfo.tempPreviewPath = filePath; // 保存原图路径用于后续传输
        qDebug() << "已插入图片预览：" << formattedFileName;
    } else if (SUPPORT_VIDEO_FORMATS.contains(fileSuffix)) {
        // 视频文件：插入视频图标+信息预览【新增：补全文件路径保存】
        fileSendInfo.fileType = "video";
        insertFilePreview(formattedFileName, formattedFileSize, "video");
        fileSendInfo.tempPreviewPath = filePath; // 【核心新增】保存视频本地路径，用于后续传输
        qDebug() << "已插入视频预览：" << formattedFileName;
    } else if (SUPPORT_COMPRESS_FORMATS.contains(fileSuffix)) {
        // 压缩文件：插入压缩包图标+信息预览【新增：补全文件路径保存】
        fileSendInfo.fileType = "compressed";
        insertFilePreview(formattedFileName, formattedFileSize, "compressed");
        fileSendInfo.tempPreviewPath = filePath; // 【核心新增】保存压缩文件本地路径，用于后续传输
        qDebug() << "已插入压缩文件预览：" << formattedFileName;
    } else {
        // 不支持的格式：提示用户
        qWarning() << "不支持的文件格式：" << fileSuffix;
        QMessageBox::information(this, "提示", "暂不支持该文件格式，仅支持图片、视频、压缩文件！");
        return;
    }

    // 5. 将文件信息存入映射表，方便后续发送文件时获取
    m_filePreviewMap[fileSendInfo.uniqueId] = fileSendInfo;

    // 6. 调试输出：查看完整的文件信息
    qDebug() << "文件信息汇总：";
    qDebug() << "  唯一ID：" << fileSendInfo.uniqueId;
    qDebug() << "  原始名称：" << fileSendInfo.originalName;
    qDebug() << "  格式化名称：" << fileSendInfo.formatName;
    qDebug() << "  格式化大小：" << fileSendInfo.formatSize;
    qDebug() << "  文件类型：" << fileSendInfo.fileType;
    qDebug() << "  本地文件路径：" << fileSendInfo.tempPreviewPath; // 【新增调试】验证路径是否正确

    // 可选：每次添加文件后，清空原有缓存（确保仅保留最新一个文件，避免累积）
    // 若需支持多文件发送，可注释此行，改为遍历m_filePreviewMap
    m_filePreviewMap.clear();
    m_filePreviewMap[fileSendInfo.uniqueId] = fileSendInfo;
}


// ---------------------- 监控聊天输入框内容变更，删除预览图时清空文件缓存 ----------------------
void MainWindow::on_chatInput_textChanged()
{
    // 1. 判断输入框中是否存在预览图（富文本图片）
    QTextDocument *doc = ui->chat_input->document();
    bool hasPreviewImage = false;

    // 遍历文档内容，判断是否包含图片（预览图都是以QTextImageFormat插入的）
    for (int i = 0; i < doc->blockCount(); ++i) {
        QTextBlock block = doc->findBlockByNumber(i);
        QTextCursor cursor(block);
        cursor.movePosition(QTextCursor::StartOfBlock);
        while (cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor)) {
            QTextCharFormat format = cursor.charFormat();
            if (format.isImageFormat()) {
                // 存在预览图，标记为true
                hasPreviewImage = true;
                break;
            }
        }
        if (hasPreviewImage) break;
    }

    // 2. 核心逻辑：如果没有预览图，且缓存不为空，清空缓存（删除预览图时触发）
    if (!hasPreviewImage && !m_filePreviewMap.isEmpty()) {
        m_filePreviewMap.clear();
        qDebug() << "预览图已删除，清空文件缓存！";
    }

    // 3. 补充：如果输入框为空，也强制清空缓存（双重保障）
    if (ui->chat_input->toPlainText().isEmpty() && ui->chat_input->document()->isEmpty()) {
        m_filePreviewMap.clear();
    }
}



// =============================如上为0110文件传输相关==============================

// ===========================如下为0110文件传输相关==============================

// 【0110 晚上新增】分片传输核心函数（主入口）
void MainWindow::sendFileByChunk(const QString &filePath, const QString &fileUniqueId, const QString &fileType)
{
    // 先清理上一次的发送残留
    if (m_sendFile) {
        if (m_sendFile->isOpen()) {
            m_sendFile->close();
        }
        delete m_sendFile;
        m_sendFile = nullptr;
    }
    
    // 保存发送文件信息到成员变量，支持异步操作
    m_sendFile = new QFile(filePath);
    if (!m_sendFile->open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "提示", "无法打开文件，无法进行分片传输！");
        delete m_sendFile;
        m_sendFile = nullptr;
        return;
    }
    
    // 获取文件基础信息
    QFileInfo fileInfo(filePath);
    qint64 fileTotalSize = fileInfo.size();
    // 计算总分片数（向上取整，避免最后一个分片不足256KB时遗漏）
    int totalChunks = (fileTotalSize + FILE_CHUNK_SIZE - 1) / FILE_CHUNK_SIZE;

    // 记录当前传输文件ID，用于后续校验
    m_currentTransferFileId = fileUniqueId;
    m_fileSendTotalSize = fileTotalSize;
    m_fileSendHasTransferred = 0;
    m_sendTotalChunks = totalChunks;
    m_sendCurrentChunkIndex = 0;
    m_sendFileType = fileType;

    // 【0320并发优化】初始化滑动窗口
    m_inFlightChunks.clear();

    // 触发第一个分片的异步发送（非阻塞，不影响聊天）
    QMetaObject::invokeMethod(this, "onAsyncSendNextChunk", Qt::QueuedConnection);
}

// 【0112修正】发送单个分片到后端
// 修正：贴合后端Message结构体，字段、类型完全对齐，后端可正常解析
bool MainWindow::sendSingleChunkToBackend(const FileChunkInfo &chunkInfo, const QByteArray &webSocketFrame)
{
    m_chunkSendWaitLock = true;
    m_lastChunkSendSuccess = false;
    
    // 保存当前分片信息
    m_currentChunkInfo = chunkInfo;
    
    // 3. 绑定bytesWritten信号
    connect(m_sslSocket, &QSslSocket::bytesWritten, this, &MainWindow::onBytesWritten, Qt::UniqueConnection);
    
    // 【0320并发优化】为每个分片创建独立的超时定时器
    int chunkIndex = chunkInfo.chunkIndex;
    if (m_chunkTimeoutTimers.contains(chunkIndex)) {
        // 清理旧的定时器
        QTimer* oldTimer = m_chunkTimeoutTimers[chunkIndex];
        if (oldTimer) {
            oldTimer->stop();
            delete oldTimer;
        }
        m_chunkTimeoutTimers.remove(chunkIndex);
    }
    
    QTimer* timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(10000); // 10秒超时
    connect(timer, &QTimer::timeout, this, [this, chunkIndex]() {
        onChunkTimeout(chunkIndex);
    });
    timer->start();
    m_chunkTimeoutTimers[chunkIndex] = timer;
    
    // 5. 发送数据（直接发送工作线程封装好的WebSocket帧）
    m_sslSocket->write(webSocketFrame);
    
    // 6. 事件驱动模式，不等待，直接返回
    return true;
}

// 重载版本：保持向后兼容
bool MainWindow::sendSingleChunkToBackend(const FileChunkInfo &chunkInfo)
{
    // 1. 封装分片信息为JSON对象（严格对应后端Message结构体，无无效字段）
    QJsonObject chunkObj;
    // 核心：消息类型标记（对应后端Type字段，值为"file_chunk"，后端可识别）
    chunkObj["type"] = "file_chunk";
    // 后端必需：发送方、接收方（对应Message.From/To）
    chunkObj["from"] = chunkInfo.fromUser;
    chunkObj["to"] = chunkInfo.toUser;
    // 后端必需：文件唯一ID（对应Message.FileID，用于后端关联分片）
    chunkObj["file_id"] = chunkInfo.fileUniqueId;
    // 分片元数据（对应Message.ChunkIndex/TotalChunks，直接传qint64数字，不转字符串）
    chunkObj["chunk_index"] = QJsonValue::fromVariant(chunkInfo.chunkIndex);
    chunkObj["total_chunks"] = QJsonValue::fromVariant(chunkInfo.totalChunks);
    // 分片数据（对应Message.ChunkData，Base64编码直接传递）
    chunkObj["chunk_data"] = chunkInfo.chunkBase64;
    // 文件元数据（对应Message.FileName/FileSize，字段名和类型对齐）
    chunkObj["file_name"] = chunkInfo.originalFileName;
    chunkObj["file_size"] = QJsonValue::fromVariant(chunkInfo.fileTotalSize);
    // 可选：标记是否为最后一个分片（后端可用于优化重组逻辑，当前后端无此字段可保留，后续扩展）
    chunkObj["is_last_chunk"] = chunkInfo.isLastChunk;
    // 可选：发送时间（后端可用于日志，非必需）
    chunkObj["send_time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    chunkObj["msg_content_type"] = chunkInfo.msgtype;

    // 2. 转为JSON字符串，通过现有WebSocket发送
    QByteArray chunkJsonBytes = QJsonDocument(chunkObj).toJson(QJsonDocument::Compact);
    
    // 3. 封装为WebSocket帧
    QByteArray maskingKey(4, 0);
    QRandomGenerator::system()->fillRange(reinterpret_cast<quint32*>(maskingKey.data()), 1);
    QByteArray frame;
    frame.append(0x81);
    
    quint64 payloadSize = chunkJsonBytes.size();
    if (payloadSize <= 125) {
        frame.append(static_cast<quint8>(payloadSize) | 0x80);
    } else if (payloadSize <= 65535) {
        frame.append(static_cast<quint8>(0x7E | 0x80));
        frame.append(static_cast<quint8>((payloadSize >> 8) & 0xFF));
        frame.append(static_cast<quint8>(payloadSize & 0xFF));
    } else {
        frame.append(static_cast<quint8>(0x7F | 0x80));
        for (int i = 7; i >= 0; --i) {
            frame.append(static_cast<quint8>((payloadSize >> (8 * i)) & 0xFF));
        }
    }
    
    frame.append(maskingKey);
    QByteArray maskedPayload = chunkJsonBytes;
    for (int i = 0; i < maskedPayload.size(); ++i) {
        maskedPayload[i] ^= maskingKey[i % 4];
    }
    frame.append(maskedPayload);
    
    // 调用新的重载版本
    return sendSingleChunkToBackend(chunkInfo, frame);
}

// 【0112修正】通知后端分片完成，所有分片发送完成后调用这个函数通知后端重组文件
// 修正：贴合后端Message结构体，字段、类型完全对齐，后端可正常识别分片完成事件
// 补充参数：int totalChunks（总分片数，从sendFileByChunk传递过来）
void MainWindow::notifyBackendFileChunkCompleted(const QString &fileUniqueId,
                                                 const QString &originalFileName,
                                                 qint64 fileTotalSize,
                                                 int totalChunks,
                                                 const QString &fileType) // 新增：总分片数参数
{
    // 1. 封装分片完成通知消息（严格对应后端Message结构体，补充total_chunks字段）
    QJsonObject completeObj;
    // 核心：消息类型标记
    completeObj["type"] = "file_chunk_complete";
    // 后端必需：发送方、接收方
    completeObj["from"] = m_currentUser;
    // 【0118批注——可优化点：这里或许应该不是当前选择的好友，理论上应该是后端把文件接收者的名字传回来】
    // 【这样写可能会导致，文件最后的接收者是当前选择的另一个好友，而不是一开始发送的那个好友】
    completeObj["to"] = m_selectedFriend;
    // 后端必需：文件唯一ID
    completeObj["file_id"] = fileUniqueId;
    // 后端必需：文件元数据
    completeObj["file_name"] = originalFileName;
    completeObj["file_size"] = QJsonValue::fromVariant(fileTotalSize);
    // 【关键修复】补充总分片数字段（和后端Message结构体的json标签一致：total_chunks）
    completeObj["total_chunks"] = QJsonValue::fromVariant(totalChunks); // 新增：传递有效总分片数
    // 可选：通知时间
    completeObj["notify_time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    // 【0112新增】补充发送文件类型
    completeObj["msg_content_type"] = fileType;
    // 2. 发送通知消息给后端
    QByteArray completeJsonBytes = QJsonDocument(completeObj).toJson(QJsonDocument::Compact);
    sendWebSocketMessage(completeJsonBytes);

    // 3. 优化日志，补充总分片数信息
    qDebug() << "【0112修正】已通知后端文件分片传输完成（文件ID：" << fileUniqueId
             << "，文件名：" << originalFileName
             << "，文件大小：" << fileTotalSize
             << "，文件类型: " << fileType
             << "字节，总分片数：" << totalChunks << "）";

    // 【0117晚上文件异步发送修改开始】
    // 发送完成后重置发送状态（异步模式下补充）
    if (m_currentTransferFileId == fileUniqueId) {
        resetFileSendState();
    }
    // 【0117晚上文件异步发送修改结束】
}

// 【0110 晚上新增】更新传输进度 updateFileTransferProgress()————排查文件传输是否有问题先看这里
// 后面确定文件传输没问题了，再弄UI
void MainWindow::updateFileTransferProgress(qint64 hasTransferred, qint64 totalSize)
{
    // 计算传输进度百分比
    int progress = (int)((double)hasTransferred / totalSize * 100);

    // 【0117晚上文件异步发送修改开始】
    // 参考下载进度条逻辑，更新fileprogress控件（发送进度）
    QMetaObject::invokeMethod(this, [=]() {
        // 校验进度条是否初始化
        if (!m_isProgressBarInited) {
            ui->fileprogress->show();
            ui->fileprogress->setRange(0, 100);
            ui->fileprogress->setFormat("文件发送进度%p%");
            m_isProgressBarInited = true;
        }

        // 限制进度在0-100之间，避免浮点计算偏差（直接使用qBound返回值，无需赋值）
        int boundedProgress = qBound(0, progress, 100); // 定义临时变量接收约束后的值（可选，更清晰）
        ui->fileprogress->setValue(boundedProgress);

        // 显示详细进度提示
        ui->fileprogress->setToolTip(QString("发送进度：%1%（%2/%3 字节）")
                                         .arg(progress)
                                         .arg(hasTransferred)
                                         .arg(totalSize));
    }, Qt::QueuedConnection);
    // 【0117晚上文件异步发送修改结束】

    qDebug() << "【0110晚上新增】传输进度：" << progress << "%";
}



// 【0113晚上新增功能：实现聊天视图点击事件，区分图片/非图片执行不同逻辑】
// 函数解析：点击聊天消息展示界面的不同项，index包含了这些项的信息，点进去就能获取其URL进行查看大图或下载文件
void MainWindow::onChatViewClicked(const QModelIndex &index)
{
    // 这部分没问题，确实进入了
    qDebug() << "进入MainWindow::onChatViewClicked函数功能";
    if (!index.isValid()) {
        return;
    }
    // qDebug() << "准备进入1. 获取消息数据";
    // 1. 获取消息数据
    // model() 方法会返回一个指向 m_chatModel 的指针
    ChatModel *chatModel = qobject_cast<ChatModel*>(ui->chat_info_show->model());
    if (!chatModel) {
        return;
    }
    // qDebug() << "准备进入2. 获取FileAttachment";
    // 2. 获取FileAttachment（通过新增的FileAttachRole）
    QVariant fileAttachVar = index.data(Qt::UserRole + 6);
    if (!fileAttachVar.isValid()) {
        qDebug() << "fileAttachVar为空";
        return;
    }
    // qDebug() << "准备进入3. 转换为FileAttachment";
    // 3. 转换为FileAttachment（需确保ChatModel支持该类型数据返回）
    FileAttachment fileAttach = fileAttachVar.value<FileAttachment>();
    if (fileAttach.previewUrl.isEmpty()) {
        qDebug() << "fileAttach为空";
        return;
    }
    // 【0114早上排查】
    // 下面两个URL是可以获取到的，没问题
    qDebug() << "大图原图URL：" << fileAttach.originUrl;
    qDebug() << "文件下载URL：" << fileAttach.downloadUrl;


    // 4. 区分图片/非图片，执行不同逻辑
    if (fileAttach.isImageFile) {
        qDebug() << "进入图片大图加载流程";
        // 图片：打开大图预览弹窗，优先使用originUrl，无则用previewUrl
        QString imageUrl = fileAttach.originUrl.isEmpty() ? fileAttach.previewUrl : fileAttach.originUrl;
        // 【0114晚上排查】用一个好友列表中发起GET请求没问题的URL来测试一下
        // QString imageUrl = "https://www.singchat.chat/avatar/1767778028_3763.jpg";
        qDebug() << "准备加载的图片URL为" << imageUrl;
        // 下面这个函数会发起下载原图的请求
        m_imagePreviewDialog->setImageUrl(imageUrl, m_sslConf);
        qDebug() << "设置URL完毕";
        m_imagePreviewDialog->show();
        qDebug() << "大图展示完毕";
    // 如果不是图片，执行上面的分支，下面这个分支就不会执行，所以不用担心图片也被下载
    // 所以说图片虽然有下载URL，但是并不会进行下载
    } else {
        // 【0117新增：下载文件进度条相关】
        // 非图片：发起文件下载
        qDebug() << "进入下载文件URL相关流程1";
        if (!fileAttach.downloadUrl.isEmpty()) {
            qDebug() << "进入下载文件URL相关流程2";

            // 【新增：下载前重置进度条】
            ui->fileprogress->reset();
            ui->fileprogress->show();
            // 自定义进度条显示文本，%p会自动替换为当前百分比（0-100）
            ui->fileprogress->setFormat("文件下载进度%p%");

            // 发起下载
            m_fileDownloader->startDownload(fileAttach.downloadUrl, m_currentUser, m_sslConf);
        } else {
            QMessageBox::warning(this, "警告", "文件下载URL为空，无法下载！");
        }
    }
}


// 【以下均为0117新增：接收文件下载进度，更新 UI 进度条】
void MainWindow::onFileDownloadProgressUpdated(qint64 bytesReceived, qint64 bytesTotal)
{
    // 确保 UI 操作在主线程（Qt 信号槽自动处理，此处做双重保障）
    QMetaObject::invokeMethod(this, [=]() {
        // ------------ 新增：首次进度更新时，执行初始化配置 ------------
        if (!m_isProgressBarInited) {
            // 1. 显示进度条
            ui->fileprogress->show();

            // 2. 重置进度条（避免下次下载残留上次进度）
            ui->fileprogress->setRange(0, 100); // 设置进度范围0-100
            ui->fileprogress->setValue(0);     // 初始进度设为0

            // 3. （若未提前设置）设置自定义文本格式
            ui->fileprogress->setFormat("文件下载进度%p%");

            // 标记为已初始化，后续进度更新不再执行此块代码
            m_isProgressBarInited = true;
        }
        // -------------------------------------------------------------

        // 你原有的逻辑（保持不变，仅需调整一处：进度值按百分比计算，适配0-100的范围）
        // 1. 处理总大小未知的情况（bytesTotal = -1）
        if (bytesTotal <= 0) {
            ui->fileprogress->setRange(0, 0); // 设置为无限滚动模式
            ui->fileprogress->setToolTip("正在下载，文件大小未知...");
            return;
        }

        // 【0117晚上修正】修正1：文件大小已知时，恢复进度条范围为0-100（避免从无限滚动模式无法切换回来）
        // 原因：若下载初期文件大小未知（range(0,0)），后续获取到文件大小后，需恢复百分比范围才能正常显示进度
        ui->fileprogress->setRange(0, 100);

        // 2. 【关键修改】将字节数转换为百分比（适配你设置的0-100范围）
        int progressPercent = (int)((double)bytesReceived / bytesTotal * 100);
        ui->fileprogress->setValue(progressPercent); // 传入百分比，而非原始字节数

        // 【0117晚上修正】修正2：限制进度百分比在0-100之间，避免因浮点计算精度问题超出范围（如101%）
        // 原因：浮点数运算可能出现微小偏差（如99.9999999转换为int是99，也可能出现100.000001转换为101），导致进度条显示异常
        progressPercent = qBound(0, progressPercent, 100);

        // 3. 【可选】显示进度百分比提示
        ui->fileprogress->setToolTip(QString("下载进度：%1%（%2/%3 字节）")
                                         .arg(progressPercent)
                                         .arg(bytesReceived)
                                         .arg(bytesTotal));
    }, Qt::QueuedConnection);
}

// 【可选：下载完成，重置进度条】
void MainWindow::onFileDownloadFinished(const QString &savePath, bool isSuccess)
{
    Q_UNUSED(savePath);
    QMetaObject::invokeMethod(this, [=]() {
        if (isSuccess) {
            ui->fileprogress->setValue(ui->fileprogress->maximum());
            // 下载完成后，隐藏进度条（可选）
            QTimer::singleShot(1000, [=]() {
                ui->fileprogress->hide();
                ui->fileprogress->reset();
                // 【0117晚上修正】修正1：重置进度条初始化标志位，确保下次下载能重新执行初始化逻辑
                // 原因：若不重置，下次下载时m_isProgressBarInited仍为true，无法显示进度条、设置格式等
                m_isProgressBarInited = false;
            });
        }
        // 【0117晚上修正】修正2：补充下载成功为false的情况，重置标志位（避免下载失败后下次下载异常）
        // 原因：若下载中途失败，isSuccess为false，需手动重置标志位，否则下次下载无法正常初始化
        else {
            m_isProgressBarInited = false;
        }
    }, Qt::QueuedConnection);
}

// 【可选：下载失败，重置进度条】
void MainWindow::onFileDownloadFailed(const QString &errorMsg)
{
    Q_UNUSED(errorMsg);
    QMetaObject::invokeMethod(this, [=]() {
        ui->fileprogress->reset();
        ui->fileprogress->hide();
        // 【0117晚上修正】修正1：重置进度条初始化标志位，为下次下载做准备
        // 原因：下载失败后，标志位仍为true，下次下载无法重新显示进度条、设置自定义格式
        m_isProgressBarInited = false;
    }, Qt::QueuedConnection);
}

// 【以上均为0117新增：接收文件下载进度，更新 UI 进度条】


// 【0117晚上文件异步发送修改开始】
void MainWindow::onAsyncSendNextChunk()
{
    // 校验：文件句柄有效、文件已打开、当前传输未被取消
    if (!m_sendFile || !m_sendFile->isOpen() || m_currentTransferFileId.isEmpty()) {
        resetFileSendState();
        return;
    }

    // 【0320并发优化】滑动窗口逻辑：并发发送多个分片
    while (m_inFlightChunks.size() < m_windowSize && m_sendCurrentChunkIndex < m_sendTotalChunks) {
        // 检查当前分片是否已经在传输中（避免重复发送）
        if (m_inFlightChunks.contains(m_sendCurrentChunkIndex)) {
            m_sendCurrentChunkIndex++;
            continue;
        }

        // 【0320进阶优化】发送信号给工作线程处理文件分片读取和WebSocket帧封装
        QFileInfo fileInfo(m_sendFile->fileName());
        emit startProcessChunk(m_sendFile, m_fileSendTotalSize, m_fileSendHasTransferred,
                              m_sendCurrentChunkIndex, m_sendTotalChunks, m_currentTransferFileId,
                              fileInfo.fileName(), m_sendFileType, m_currentUser, m_selectedFriend);

        // 标记当前分片为已发送（加入窗口）
        m_inFlightChunks.insert(m_sendCurrentChunkIndex);

        // 更新发送索引
        m_sendCurrentChunkIndex++;

        qDebug() << "【0320并发优化】发送分片" << (m_sendCurrentChunkIndex - 1) 
                 << "，窗口中分片数：" << m_inFlightChunks.size() << "/" << m_windowSize;
    }

    // 检查是否所有分片都已发送
    if (m_sendCurrentChunkIndex >= m_sendTotalChunks && m_inFlightChunks.isEmpty()) {
        // 所有分片发送完成，通知后端重组
        QFileInfo fileInfo(m_sendFile->fileName());
        notifyBackendFileChunkCompleted(m_currentTransferFileId,
                                        fileInfo.fileName(),
                                        m_fileSendTotalSize,
                                        m_sendTotalChunks,
                                        m_sendFileType);
        QMessageBox::information(this, "提示", "文件分片传输完成，等待后端重组！");
        resetFileSendState();
    }
}
// 【0117晚上文件异步发送修改结束】

// 【0117晚上文件异步发送修改开始】
void MainWindow::resetFileSendState()
{
    // 关闭并清理文件句柄
    if (m_sendFile) {
        if (m_sendFile->isOpen()) {
            m_sendFile->close();
        }
        delete m_sendFile;
        m_sendFile = nullptr;
    }

    // 清理超时定时器
    if (m_chunkTimeoutTimer) {
        m_chunkTimeoutTimer->stop();
        delete m_chunkTimeoutTimer;
        m_chunkTimeoutTimer = nullptr;
    }

    // 【0320并发优化】清理所有分片的超时定时器
    for (auto it = m_chunkTimeoutTimers.begin(); it != m_chunkTimeoutTimers.end(); ++it) {
        QTimer* timer = it.value();
        if (timer) {
            timer->stop();
            delete timer;
        }
    }
    m_chunkTimeoutTimers.clear();

    // 【0320并发优化】清空滑动窗口
    m_inFlightChunks.clear();

    // 断开bytesWritten信号连接
    disconnect(m_sslSocket, &QSslSocket::bytesWritten, this, &MainWindow::onBytesWritten);

    // 重置发送状态变量
    m_fileSendTotalSize = 0;
    m_fileSendHasTransferred = 0;
    m_sendTotalChunks = 0;
    m_sendCurrentChunkIndex = 0;
    m_sendFileType = "";
    m_currentTransferFileId = "";
    m_currentChunkSize = 0;

    // 重置进度条
    QMetaObject::invokeMethod(this, [=]() {
        ui->fileprogress->setValue(100);
        QTimer::singleShot(1000, [=]() {
            ui->fileprogress->hide();
            ui->fileprogress->reset();
            m_isProgressBarInited = false;
        });
    }, Qt::QueuedConnection);

    qDebug() << "【0117异步发送】文件发送状态已重置";
}
// 【0117晚上文件异步发送修改结束】

// 【事件驱动优化】处理bytesWritten信号
void MainWindow::onBytesWritten(qint64 /*bytes*/)
{
    // 【0320并发优化】在并发模式下，不再需要断开信号连接
    // 因为每个分片都有独立的超时定时器，通过后端确认来触发下一个分片发送
    
    // 数据已写入套接字缓冲区，等待后端确认
    // 注意：不再在这里处理重传逻辑，重传由onChunkTimeout处理
    // 分片发送成功的处理已移到收到后端确认消息时
}


// 【事件驱动优化】处理分片超时
void MainWindow::onChunkTimeout()
{
    qWarning() << "【0117异步发送】分片" << m_currentChunkInfo.chunkIndex << "发送超时";
    
    // 断开信号连接
    disconnect(m_sslSocket, &QSslSocket::bytesWritten, this, &MainWindow::onBytesWritten);
    
    // 清理定时器
    if (m_chunkTimeoutTimer) {
        delete m_chunkTimeoutTimer;
        m_chunkTimeoutTimer = nullptr;
    }
    
    // 尝试重传
    int retryCount = 0;
    while (retryCount < 3 && m_currentTransferFileId == m_currentChunkInfo.fileUniqueId) {
        qWarning() << "【0117异步发送】分片" << m_currentChunkInfo.chunkIndex << "超时，尝试重传...";
        sendSingleChunkToBackend(m_currentChunkInfo);
        retryCount++;
    }
    
    if (m_currentTransferFileId == m_currentChunkInfo.fileUniqueId) {
        QMessageBox::warning(this, "提示", QString("分片%1发送超时（多次重传无效），传输终止！").arg(m_currentChunkInfo.chunkIndex));
        resetFileSendState();
    }
}

// 【0320并发优化】处理分片超时（带参数版本）
void MainWindow::onChunkTimeout(int chunkIndex)
{
    qWarning() << "【0320并发优化】分片" << chunkIndex << "发送超时";
    
    // 从窗口中移除超时的分片
    m_inFlightChunks.remove(chunkIndex);
    
    // 清理对应的定时器
    if (m_chunkTimeoutTimers.contains(chunkIndex)) {
        QTimer* timer = m_chunkTimeoutTimers[chunkIndex];
        if (timer) {
            timer->stop();
            delete timer;
        }
        m_chunkTimeoutTimers.remove(chunkIndex);
    }
    
    // 尝试重传（重置发送索引）
    m_sendCurrentChunkIndex = chunkIndex;
    
    // 触发重新发送
    QMetaObject::invokeMethod(this, "onAsyncSendNextChunk", Qt::QueuedConnection);
}

// 【0320进阶优化】处理工作线程分片处理完成信号
void MainWindow::onChunkProcessed(const FileChunkInfo &chunkInfo, const QByteArray &/*webSocketFrame*/)
{
    // 保存当前分片大小
    m_currentChunkSize = chunkInfo.chunkBase64.size() * 3 / 4; // Base64编码后的大小转换为原始大小
    
    // 发送当前分片（事件驱动，不阻塞）
    sendSingleChunkToBackend(chunkInfo);
}

// 【0320进阶优化】处理工作线程分片处理错误信号
void MainWindow::onProcessError(const QString &errorMessage)
{
    qWarning() << "【0320工作线程】" << errorMessage;
    QMessageBox::warning(this, "提示", errorMessage);
    resetFileSendState();
}


// 【0119晚上群聊相关开始：修改MainWindow传参，仅传入SSL相关参数】
void MainWindow::on_btnCreateGroup_clicked()
{
    if (!m_createGroupDialog) {
        // 传入与注册逻辑一致的参数（复刻注册逻辑，保证可行性）
        m_createGroupDialog = new CreateGroup(
            m_netManager,        // 复用HTTP管理器（与注册共用一个，避免重复创建）
            m_sslConf,           // 通用SSL配置（与注册共用，保证一致性）
            m_sslSocket,         // 主窗口已完成握手的SSL套接字（复用发送消息）
            m_currentUser,       // 当前登录用户（与注册一致）
            this                 // 父窗口
            );
    }

    // 1. 传递好友列表模型和委托（根据你的实际列表修改）
    QAbstractItemModel *personListModel = ui->person_list->model();
    QAbstractItemDelegate *personListDelegate = ui->person_list->itemDelegate();
    m_createGroupDialog->setGroupListModelAndDelegate(personListModel, personListDelegate);

    // 2. 窗口尺寸刷新（复刻注册逻辑，确保布局生效）
    m_createGroupDialog->layout()->activate();
    m_createGroupDialog->adjustSize();

    // 3. 显示创建群聊窗口
    m_createGroupDialog->show();
}
// 【0119晚上群聊相关结束】


// 【0123白天群聊/私聊按钮红点显示逻辑开始】
// 【0123白天群聊/私聊按钮红点显示逻辑开始】
// 【0127晚上最终优化：动态尺寸+统一显示规则+文本精准居中】
void MainWindow::showChatGroupChatRedDot(int chatUnreadCount, int groupChatUnreadCount)
{
    QMetaObject::invokeMethod(this, [=]() {
        // ========== 通用红点参数定义（完全参照FriendListDelegate样式） ==========
        const int badgeMinSize = 18;    // 基础尺寸：高度固定，1位数字宽度
        const int badgeWidth2Digits = 24;// 2位数字宽度
        const int badgeWidthMore = 30;  // 99+宽度
        const int badgeOffset = 2;      // 红点与按钮边缘的偏移量

        // ========== 处理私聊按钮红点 ==========
        if (chatUnreadCount > 0) {
            QString chatDisplayText;
            int chatBadgeWidth = badgeMinSize;
            // 确定显示文本+控件宽度
            if (chatUnreadCount > 99) {
                chatDisplayText = "99+";
                chatBadgeWidth = badgeWidthMore;
            } else {
                chatDisplayText = QString::number(chatUnreadCount);
                if (chatUnreadCount >= 10) {
                    chatBadgeWidth = badgeWidth2Digits;
                }
            }
            // 设置文本+固定尺寸
            m_chatBadge->setText(chatDisplayText);
            m_chatBadge->setFixedSize(chatBadgeWidth, badgeMinSize);
            // 【0127晚上新增：核心居中逻辑】文本水平+垂直居中（彻底解决偏位）
            m_chatBadge->setAlignment(Qt::AlignCenter);
            // 计算按钮右下角坐标（适配动态宽度）
            QRect chatBtnRect = ui->btnStartChat->rect();
            int chatX = chatBtnRect.width() - chatBadgeWidth - badgeOffset;
            int chatY = chatBtnRect.height() - badgeMinSize - badgeOffset;
            m_chatBadge->setGeometry(chatX, chatY, chatBadgeWidth, badgeMinSize);
            m_chatBadge->show();
        } else {
            m_chatBadge->hide();
        }

        // ========== 处理群聊按钮红点（与私聊完全统一规则） ==========
        if (groupChatUnreadCount > 0) {
            QString groupDisplayText;
            int groupBadgeWidth = badgeMinSize;
            // 确定显示文本+控件宽度（无9+特殊逻辑，和私聊完全一致）
            if (groupChatUnreadCount > 99) {
                groupDisplayText = "99+";
                groupBadgeWidth = badgeWidthMore;
            } else {
                groupDisplayText = QString::number(groupChatUnreadCount);
                if (groupChatUnreadCount >= 10) {
                    groupBadgeWidth = badgeWidth2Digits;
                }
            }
            // 设置文本+固定尺寸
            m_groupChatBadge->setText(groupDisplayText);
            m_groupChatBadge->setFixedSize(groupBadgeWidth, badgeMinSize);
            // 【0127晚上新增：核心居中逻辑】文本水平+垂直居中
            m_groupChatBadge->setAlignment(Qt::AlignCenter);
            // 计算按钮右下角坐标（适配动态宽度）
            QRect groupBtnRect = ui->groupchat->rect();
            int groupX = groupBtnRect.width() - groupBadgeWidth - badgeOffset;
            int groupY = groupBtnRect.height() - badgeMinSize - badgeOffset;
            m_groupChatBadge->setGeometry(groupX, groupY, groupBadgeWidth, badgeMinSize);
            m_groupChatBadge->show();
        } else {
            m_groupChatBadge->hide();
        }

        qDebug() << "✅ 私聊/群聊红点已更新：私聊总未读=" << chatUnreadCount << "，群聊总未读=" << groupChatUnreadCount;
    }, Qt::QueuedConnection);
}
// 【0123白天红点显示逻辑结束】

// 【0201白天QT心跳包逻辑开始：处理Socket主动断开】
void MainWindow::onWebSocketDisconnected() {
    QMessageBox::warning(this, "提示", "连接已断开！");

    qDebug() << "\n🔌 WebSocket连接已断开";
    m_isWebSocketConnected = false;

    // 停止所有定时器（避免定时器继续触发）
    if (m_heartbeatSendTimer && m_heartbeatSendTimer->isActive()) {
        m_heartbeatSendTimer->stop();
    }
    if (m_pongTimeoutTimer && m_pongTimeoutTimer->isActive()) {
        m_pongTimeoutTimer->stop();
    }
}
// 【0201白天QT心跳包逻辑结束】

// 发送ping心跳包（m_heartbeatSendTimer触发）
void MainWindow::sendPingMessage() {
    if (!m_isWebSocketConnected) {
        qDebug() << "⚠️ 连接已断开，跳过ping发送";
        return;
    }

    // 构造ping消息（和服务端routeMessage中的ping分支对应）
    QJsonObject pingMsgObj;
    pingMsgObj["type"] = "ping";
    pingMsgObj["from"] = m_currentUser; // 当前登录用户名
    pingMsgObj["to"] = "server";        // 发送给服务端
    pingMsgObj["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    pingMsgObj["content"] = "ping";     // 心跳内容

    // 发送ping消息（复用你现有sendWebSocketMessage函数）
    sendWebSocketMessage(QJsonDocument(pingMsgObj).toJson(QJsonDocument::Compact));
    qDebug() << "\n📤 发送ping心跳包：" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    // 启动pong超时计时器（60秒内没收到pong则认为连接断开）
    if (m_pongTimeoutTimer) {
        m_pongTimeoutTimer->start();
    }
}

// pong超时处理（60秒没收到pong，认为连接断开）
void MainWindow::onPongTimeout() {
    qDebug() << "\n❌ pong超时（60秒未收到服务端响应），标记连接断开";
    m_isWebSocketConnected = false;
}

// 【0201白天QT心跳包逻辑开始：处理Socket错误】
void MainWindow::onSocketError(QAbstractSocket::SocketError error) {
    QString errorDesc;
    switch (error) {
    case QAbstractSocket::ConnectionRefusedError: errorDesc = "服务器拒绝连接"; break;
    case QAbstractSocket::RemoteHostClosedError: errorDesc = "远程主机关闭连接"; break;
    case QAbstractSocket::NetworkError: errorDesc = "网络异常（断网/超时）"; break;
    default: errorDesc = "未知错误：" + QString::number(error);
    }

    // ========== 核心：fd=-1 或 Socket为空时，只打日志不操作 ==========
    qintptr currentFd = m_sslSocket ? m_sslSocket->socketDescriptor() : -1;
    qCritical() << "❌ Socket错误：" << errorDesc << " | 当前fd=" << currentFd;

    if (!m_sslSocket || currentFd == -1) {
        qWarning() << "⚠️ Socket已释放，跳过错误处理的Socket操作";
        onWebSocketDisconnected(); // 只触发断开逻辑，不操作Socket
        return;
    }

    // ========== 操作Socket前加防护 ==========
    m_sslSocket->blockSignals(true);
    onWebSocketDisconnected();
    m_sslSocket->blockSignals(false);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    // 保存当前滚动位置
    QScrollBar *scrollBar = ui->chat_info_show->verticalScrollBar();
    int scrollPosition = scrollBar->value();
    
    // 调用父类的resizeEvent处理布局
    QMainWindow::resizeEvent(event);
    
    // 恢复滚动位置
    QTimer::singleShot(0, [=]() {
        scrollBar->setValue(scrollPosition);
    });
}

void MainWindow::focusOutEvent(QFocusEvent *event)
{
    // 直接调用父类的focusOutEvent，不进行滚动位置的保存和恢复
    QMainWindow::focusOutEvent(event);
}



void MainWindow::onChatViewLeave()
{
    // 禁用此方法，防止鼠标离开时滚动条跳动
    // 此方法可能由UI信号自动连接，所以保留空实现
}

// 懒加载实现：滚动到顶部时加载更多聊天记录
void MainWindow::onChatViewScrollValueChanged(int value)
{
    // 检查是否接近顶部且有更多消息可加载
    // 添加额外检查：确保聊天视图有焦点且滚动事件是用户主动触发
    if (value <= 5 && !m_isLoading && m_chatModel->hasMoreMessages() && ui->chat_info_show->hasFocus()) {
        m_isLoading = true; // 设置加载状态，防止重复加载
        
        // 准备加载更多消息
        
        // 显示加载提示（在聊天视图底部添加加载中消息）
        ChatMessage loadingMsg;
        loadingMsg.content = "正在加载更多聊天记录...";
        loadingMsg.owner = ChatMessage::System;
        loadingMsg.type = ChatMessage::Text;
        loadingMsg.senderName = "系统";
        loadingMsg.avatarPath = "";
        loadingMsg.timestamp = QDateTime::currentDateTime();
        m_chatModel->addMessage(loadingMsg);
        
        // 向后端发送获取历史消息的请求
        QJsonObject requestObj;
        requestObj["type"] = "get_chat_records";
        requestObj["from"] = m_currentUser;
        requestObj["to"] = m_selectedFriend;
        requestObj["pull_num"] = 20; // 每次加载20条历史消息
        
        // 获取当前最早消息的时间戳，用于向后端请求更早的消息
        if (m_chatModel->rowCount() > 0) {
            QVector<ChatMessage> messages = m_chatModel->messages();
            if (!messages.isEmpty() && messages.size() > 1) { // 排除刚刚添加的加载提示
                ChatMessage oldestMsg = messages.first();
                requestObj["msg_ts"] = static_cast<qint64>(oldestMsg.timestamp.toMSecsSinceEpoch());
            } else if (!messages.isEmpty()) {
                // 如果只有加载提示消息，使用当前时间
                requestObj["msg_ts"] = QDateTime::currentDateTime().toMSecsSinceEpoch();
            }
        } else {
            requestObj["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        }
        
        // 发送WebSocket请求
        QJsonDocument doc(requestObj);
        sendWebSocketMessage(doc.toJson(QJsonDocument::Compact));
        qDebug() << "🔍 发送查询历史聊天记录请求：" << m_currentUser << " ↔ " << m_selectedFriend;
    }
}
// 【0201白天QT心跳包逻辑结束】
