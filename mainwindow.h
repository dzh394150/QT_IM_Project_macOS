#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSslSocket>
#include <QSslConfiguration>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>
#include <QModelIndex>
#include <QMetaObject>
#include <QRandomGenerator>
#include <QByteArray>
#include <QString>
#include <QList>
#include <QLabel>
#include <QListView>
// 包含模型头文件（获取FriendData/FriendRequestData定义）
#include "FriendListDelegate.h"
#include "FriendListModel.h"
#include "FriendRequestModel.h"
#include "chatmodel.h"
#include "chatdelegate.h"
#include <QTextEdit>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDragMoveEvent>
#include <QStringList>
#include <QMap>
#include <QUuid>
#include <QFileInfo>
#include <QMessageBox>
#include <QMimeData>
#include <QUrl>
#include <QString>
#include <QClipboard>
#include <QMimeData>
#include <QThread>

#include <ImagePreviewDialog.h>
#include <FileDownloader.h>
#include "filetransferworker.h"

#include "CreateGroup.h"  // 引入CreateGroup头文件
#include <QPointer>       // 可选：使用QPointer避免悬空指针（更安全，推荐）
#include <GroupListModel.h>
#include <GroupListDelegate.h>
#include "LocalChatCache.h"
// 前置声明
class AddFriends;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// ----------------------0110文件传输功能——定义FileSendInfo结构体 ----------------------
struct FileSendInfo {
    QString uniqueId;       // 文件唯一标识（用QUuid生成，避免重复）
    QString originalPath;   // 原文件完整本地路径（如D:/xxx/xxx.mp4）
    QString originalName;   // 原文件完整名称（含后缀）
    QString formatName;     // 格式化后的文件名（超过20字符首尾各留8位，中间....）
    QString formatSize;     // 格式化后的文件大小（KB/MB/GB，保留1位小数）
    QString fileType;       // 文件类型（image/video/compressed，对应msg_content_type）
    QString tempPreviewPath;// 预览图临时文件路径（拼接后的图片，插入QTextEdit用）
    QString fileSuffix;     // 【新增】文件后缀（.png/.mp4/.zip，方便后端转发识别）
};

#include "FileChunkInfo.h"

// ----------------------0110文件传输功能——自定义ChatInputTextEdit子类（继承QTextEdit，添加在MainWindow类之前） ----------------------
class ChatInputTextEdit : public QTextEdit
{
    Q_OBJECT // 必须添加Q_OBJECT宏，支持信号与槽机制

public:
    // 构造函数
    explicit ChatInputTextEdit(QWidget *parent = nullptr) : QTextEdit(parent) {
        // 开启拖拽接收功能（默认QTextEdit可能关闭，需手动开启）
        setAcceptDrops(true);
        // 【删除】构造函数中不再缓存样式表（时机过早）
        // m_originalStyleSheet = this->styleSheet();
    }
    // 【新增】公共接口：手动缓存当前样式表（延后到样式加载完成后调用）
    void cacheOriginalStyleSheet() {
        m_originalStyleSheet = this->styleSheet();
    }

    // 从剪贴板提取图片文件路径
    QString getImagePathFromClipboard();




protected:
    // 重写：拖拽进入事件
    void dragEnterEvent(QDragEnterEvent *event) override;

    // 重写：拖拽移动事件
    void dragMoveEvent(QDragMoveEvent *event) override;

    // 重写：拖拽放下事件
    void dropEvent(QDropEvent *event) override;

    // 重写：声明拖拽离开事件重写
    void dragLeaveEvent(QDragLeaveEvent *event) override;

    // 重写：声明键盘按下事件重写（监听Ctrl+V粘贴）
    void keyPressEvent(QKeyEvent *event) override;

private:
    // 辅助函数：判断文件是否为支持的格式（内部使用，用于拖拽过滤）
    bool isSupportFileFormat(const QString &filePath);

    // 成员变量：缓存原始样式表，用于拖拽后还原
    QString m_originalStyleSheet;

    //【0117晚上文件异步发送修改开始】
    // 异步分片发送缓存信息（用于边发送边聊天，不阻塞UI）
    QFile *m_currentSendFile;          // 当前发送的文件句柄（动态分配，避免栈溢出）
    int m_currentSendChunkIndex;       // 当前待发送的分片索引（从0开始）
    qint64 m_currentSendFileTotalSize; // 当前发送文件的总大小（字节）
    QString m_currentSendFileName;     // 当前发送文件的原始名称
    QString m_currentSendFileType;     // 当前发送文件的类型（image/video/compressed）
    int m_currentSendTotalChunks;      // 当前发送文件的总分片数
    //【0117晚上文件异步发送修改结束】



signals:
    // 自定义信号：拖拽放下成功后，发送文件路径给MainWindow处理后续预览逻辑
    void fileDropped(const QString &filePath);

};




class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    // ===============0110文件传输相关=========================
    // 【新增】映射表：关联预览图唯一标识与文件核心信息
    QMap<QString, FileSendInfo> m_filePreviewMap;

    // 【新增】支持的文件格式集合（静态常量，所有实例共享）
    static const QStringList SUPPORT_IMAGE_FORMATS;
    static const QStringList SUPPORT_VIDEO_FORMATS;
    static const QStringList SUPPORT_COMPRESS_FORMATS;


    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setLoginInfo(const QString &username, QSslSocket *sslSocket);

    // 【0110晚上新增】文件分片传输配置常量（1MB/片，可调整）
    static const qint64 FILE_CHUNK_SIZE;
public slots:
    // 【0123未读消息逻辑开始：声明新的红点显示函数】
    void showChatGroupChatRedDot(int chatUnreadCount, int groupChatUnreadCount);
    // 【0123未读消息逻辑结束】

private slots:
    void onSslReadyRead();
    void onWebSocketDisconnected();
    void onPersonListItemClicked(const QModelIndex &index);
    void onFriendRequestListItemClicked(const QModelIndex &index);
    void onSendButtonClicked();
    void onBtnLookFriendClicked();
    void onBtnAgreeClicked();
    void onBtnRefuseClicked();
    void onSslErrors(QNetworkReply *reply, const QList<QSslError> &errors);
    void toggleMaximize();
    // 新增
    void onBtnAddFriendsClicked();
    void onBtnStartChatClicked();
    void loadUserAvatar(const QString &avatarUrl);
    void onAvatarLoaded(QNetworkReply *reply);
    // 新增：显示/隐藏聊天滚动条
    void showListViewScrollBar(QListView *listView, bool show);
    void onChatViewScrollValueChanged(int value);

    // --------------------0110新增文件传输功能----------------------
    // 槽函数：接收ChatInputTextEdit发送的文件路径，后续处理预览生成（暂时先声明，后续实现）
    void onFileDropped(const QString &filePath);
    // 监控聊天输入框内容变更，预览图被删掉的时候清空缓存
    void on_chatInput_textChanged();

    // 【0113晚上新增功能：声明聊天视图点击事件槽函数，处理预览图点击逻辑】
    void onChatViewClicked(const QModelIndex &index);
    // 【0117新增，文件下载进度条相关】
    void onFileDownloadProgressUpdated(qint64 bytesReceived, qint64 bytesTotal);
    void onFileDownloadFinished(const QString &savePath, bool isSuccess);
    void onFileDownloadFailed(const QString &errorMsg);
    void resizeEvent(QResizeEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event);
    void onChatViewLeave();

    // 【0117晚上文件异步发送修改开始】
    // 新增：异步发送下一个分片的槽函数（非阻塞，支持同时聊天）
    void onAsyncSendNextChunk();
    // 新增：重置文件发送状态（清理资源、隐藏进度条）
    void resetFileSendState();
    // 【事件驱动优化】新增槽函数
    void onBytesWritten(qint64 bytes);
    void onChunkTimeout();
    // 【0320并发优化】新增槽函数：处理分片超时（带参数版本）
    void onChunkTimeout(int chunkIndex);
    // 【0320进阶优化】工作线程相关槽函数
    void onChunkProcessed(const FileChunkInfo &chunkInfo, const QByteArray &webSocketFrame);
    void onProcessError(const QString &errorMessage);
    // 【0117晚上文件异步发送修改结束】

    // 【0118群聊按钮槽函数】
    void ongroupchatClicked();

    // 【0118晚上群聊功能实现开始】
    void on_btnCreateGroup_clicked();  // 创建群聊按钮点击槽函数
    // 【0118晚上群聊功能实现结束】

    void onGroupListItemClicked(const QModelIndex &index);


private:
    void sendWebSocketMessage(const QString &message);
    // 【0112修改帧解析函数】
    QString parseWebSocketFrame(const QByteArray &frame, bool &isFinalFrame, QByteArray &framePayload);
    void addMessageToChatShow(const QString &sender, const QString &content, const QString &time);
    void loadFriendList(const QList<FriendData> &friendList); // FriendData已在FriendListModel.h中定义
    void loadFriendRequests();
    void showFriendRequestRedDot(int unreadCount);
    void sendFriendRequestAgree(const QString &fromUser);
    void sendFriendRequestRefuse(const QString &fromUser);

    QSslConfiguration getSslConfig();
    QList<QSslError> ignoreCriticalSslErrors(const QList<QSslError> &errors, QString &errorSummary);


    Ui::MainWindow *ui;
    QSslSocket *m_sslSocket;
    QSslConfiguration m_sslConf;
    QNetworkAccessManager *m_netManager;
    QString m_currentUser;
    QString m_selectedFriend;
    FriendListModel *m_friendListModel;
    FriendRequestModel *m_friendRequestModel;
    QString m_selectedRequestFromUser;
    ChatModel *m_chatModel;
    // 新增
    AddFriends *m_addFriendsDialog;


    bool isInTitleBar(const QPoint &pos);   // 判断鼠标是否在标题栏区域
    int getResizeEdge(const QPoint &pos);   // 判断鼠标是否在窗口边缘
    void updateCursorShape(const QPoint &pos); // 更新光标形状
    void handleResize(QMouseEvent *event); // 处理缩放逻辑

    bool isDragging;            // 是否正在拖动窗口
    bool isResizing;            // 是否正在缩放窗口
    int resizeEdge;             // 缩放的边缘
    QPoint dragStartPosition;   // 拖动的起始位置
    QPoint resizeStartPosition; // 缩放的起始位置
    QRect resizeStartGeometry;  // 缩放的起始几何形状

    // 【0110晚上新增】分片传输相关成员变量
    bool m_chunkSendWaitLock;       // 分片发送等待锁（确保串行发送）
    bool m_lastChunkSendSuccess;    // 最后一个分片发送是否成功
    QString m_currentTransferFileId; // 当前正在传输的文件唯一ID
    // 【0117晚上文件异步发送修改开始】
    // 新增：文件发送异步相关成员变量
    QFile* m_sendFile;              // 发送文件句柄（改为成员，支持异步操作）
    qint64 m_fileSendTotalSize;     // 发送文件总字节数
    qint64 m_fileSendHasTransferred;// 已发送字节数
    int m_sendTotalChunks;          // 发送总分片数
    int m_sendCurrentChunkIndex;    // 当前发送分片索引
    QString m_sendFileType;         // 当前发送文件类型
    // 【事件驱动优化】新增成员变量
    qint64 m_currentChunkSize;      // 当前分片大小
    QTimer* m_chunkTimeoutTimer;    // 分片超时定时器
    FileChunkInfo m_currentChunkInfo; // 当前正在发送的分片信息
    // 【0320并发优化】滑动窗口相关成员变量
    const int m_windowSize = 4;     // 滑动窗口大小（并发发送4个分片）
    QSet<int> m_inFlightChunks;     // 已发送但未确认的分片索引集合
    QMap<int, QTimer*> m_chunkTimeoutTimers; // 每个分片的超时定时器映射
    // 【0320进阶优化】工作线程相关
    QThread *m_fileTransferThread; // 文件传输工作线程
    FileTransferWorker *m_fileTransferWorker; // 文件传输工作线程对象
    // 【0117晚上文件异步发送修改结束】

    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    
    // 0106新增：自己和好友的聊天头像
    QString m_myAvatarUrl; // 自己的头像 URL
    QMap<QString, QString> m_friendAvatars; // 好友头像缓存

    // 0107新增
    QLabel *m_notificationBadge; // 好友请求红点标签

    // ========================0110新增文件传输逻辑=========================
    // 【新增】辅助函数：格式化文件大小（字节→KB/MB/GB，保留1位小数）
    QString formatFileSize(qint64 fileSizeBytes);

    // 【新增】辅助函数：格式化过长文件名（超过20字符首尾各8位，中间加....）
    QString formatFileName(const QString &originalName);

    // 【新增】辅助函数：在chat_input中插入图片预览
    void insertImagePreview(const QString &filePath, const QString &fileName);

    // 【新增】辅助函数：在chat_input中插入视频/压缩文件的文字+图标预览
    void insertFilePreview(const QString &fileName, const QString &fileSize, const QString &fileType);




    QByteArray m_frameCache; // 【0112新增】全局缓存，用于拼接分批到达的TCP数据，解决截断问题
    QByteArray m_messageFragmentCache; // 【0112新增】分片缓存：拼接WebSocket大消息的所有分片payload
    bool m_isReceivingFragmentedMessage; // 【0112新增】标记：是否正在接收分片消息

    // 【0112下午新增】：分片传输缓存相关变量
    QByteArray m_fragmentPayloadCache; // 缓存分片传输的所有 payload 数据
    bool m_isInFragmentTransfer;       // 标记是否处于分片传输中
    quint8 m_fragmentInitOpcode;       // 记录分片传输的初始帧 opcode
    // 【0112下午新增】
    QString parseWebSocketFrame(const QByteArray &frame, bool &isFinalFrame, quint8 &opcode, QByteArray &framePayload);

    // 【0112新增】辅助函数：计算完整WebSocket帧的长度（判断缓存是否足够）
    quint64 calculateCompleteWebSocketFrameLength(const QByteArray &cache);

    // 【0110晚上新增】分片传输核心函数（主入口）
    void sendFileByChunk(const QString &filePath, const QString &fileUniqueId, const QString &fileType);

    // 【0110晚上新增】发送单个分片到后端（含等待确认逻辑）
    bool sendSingleChunkToBackend(const FileChunkInfo &chunkInfo);
    // 【0320进阶优化】发送单个分片到后端（直接使用工作线程封装好的WebSocket帧）
    bool sendSingleChunkToBackend(const FileChunkInfo &chunkInfo, const QByteArray &webSocketFrame);

    // 【0112修正】通知后端所有分片发送完成，可进行重组
    // 新增：总分片数参数
    void notifyBackendFileChunkCompleted(const QString &fileUniqueId,const QString &originalFileName,qint64 fileTotalSize,int totalChunks,const QString &fileType);

    // 【0110晚上新增】更新文件传输进度（UI反馈，可选）
    void updateFileTransferProgress(qint64 hasTransferred, qint64 totalSize);

    ImagePreviewDialog *m_imagePreviewDialog;  // 【0113晚上新增功能：声明大图预览弹窗成员变量】
    FileDownloader *m_fileDownloader;          // 【0113晚上新增功能：声明文件下载工具类成员变量】
    bool m_isProgressBarInited = false; // 【0117文件传输相关】进度条是否已初始化（默认false）
    CreateGroup *m_createGroupDialog;

    // 【0120白天群聊列表渲染相关逻辑开始：声明群聊列表模型和委托对象】
    GroupListModel *m_groupListModel;
    GroupListDelegate *m_groupListDelegate;
    // 【0120白天群聊列表渲染相关逻辑结束】

    // 【0123未读消息逻辑开始：新增私聊/群聊红点控件】
    QLabel *m_chatBadge = nullptr;    // 私聊按钮红点
    QLabel *m_groupChatBadge = nullptr;// 群聊按钮红点
    // 【0123未读消息逻辑结束】

    FriendListDelegate *m_friendDelegate;

    QTimer* m_heartbeatSendTimer;    // 定时发送ping的计时器（30秒一次）
    QTimer* m_pongTimeoutTimer;      // pong超时检测计时器（60秒）
    bool m_isWebSocketConnected;     // WebSocket连接状态标记
    bool m_isLoading;                // 懒加载状态标记，防止重复加载
    void sendPingMessage();
    void onPongTimeout();
    void onSocketError(QAbstractSocket::SocketError error);

signals:
    // 【0119晚上新增】自定义信号，转发套接字数据给群聊窗口（参数为读取到的原始字节数组）
    void signalGroupSocketDataArrived(const QByteArray &socketData);
    // 【0320进阶优化】工作线程相关信号
    void startProcessChunk(QFile *file, qint64 fileTotalSize, qint64 hasTransferred, 
                          int chunkIndex, int totalChunks, const QString &fileUniqueId,
                          const QString &fileName, const QString &fileType, 
                          const QString &fromUser, const QString &toUser);

};



#endif // MAINWINDOW_H
