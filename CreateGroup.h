#ifndef CREATEGROUP_H
#define CREATEGROUP_H

// 必要头文件（与注册逻辑对齐）
#include <QDialog>
#include <QPoint>
#include <QMessageBox>
#include <QList>
#include <QSslError>
#include <QSslSocket>
#include <QSslConfiguration>
#include <QNetworkAccessManager>

// 前置声明
class QAbstractItemModel;
class QAbstractItemDelegate;
namespace Ui { class CreateGroup; }

class CreateGroup : public QDialog
{
    Q_OBJECT // QT槽函数必备宏

public:
    // 构造函数（与注册逻辑对齐，传入主窗口现成SSL套接字+通用参数）
    explicit CreateGroup(
        QNetworkAccessManager *netManager,       // 复用HTTP管理器（与注册逻辑一致）
        const QSslConfiguration &sslConf,         // 通用SSL配置（与注册逻辑一致）
        QSslSocket *mainSslSocket,               // 主窗口已完成握手的SSL套接字（复用发送消息）
        const QString &currentUser,               // 当前登录用户
        QWidget *parent = nullptr
        );
    ~CreateGroup();

    // 公共接口：设置列表模型和委托
    void setGroupListModelAndDelegate(QAbstractItemModel *model, QAbstractItemDelegate *delegate);
    void setTargetWindowPos(const QPoint &btnScreenPos);

private slots:
    // 窗口基础操作
    void on_groupclose_clicked();
    void doMoveToTargetPos();

    // 业务操作（与注册逻辑对齐）
    void selectAvatar();                // 选择头像（复刻注册逻辑）
    void handleCreateGroup();           // 处理创建群聊（先上传头像，再发消息）

    // SSL相关槽函数（与注册逻辑对齐）
    void onSslErrors(const QList<QSslError> &errors);
    // void onSslReadyRead();              // 复用主窗口SSL套接字接收响应
    void onDisconnected();              // 连接断开处理
    void onSocketError(QAbstractSocket::SocketError error); // 套接字错误处理

    // 0119新增：接收主窗口转发数据的槽函数（替换原有直接绑定readyRead的逻辑）
    void onRecvMainWindowSocketData(const QByteArray &socketData);

private:
    // UI与核心成员变量（与注册逻辑对齐）
    Ui::CreateGroup *ui;
    QPoint m_targetPos;
    QPoint m_dragPos;
    bool m_isDragging;

    // 复用与配置相关（与注册逻辑对齐）
    QNetworkAccessManager *m_netManager;        // HTTP管理器（与注册逻辑一致，用于上传头像）
    QSslConfiguration m_sslConf;                // 通用SSL配置（与注册逻辑一致）
    QSslSocket *m_mainSslSocket;                // 主窗口现成SSL套接字（复用发送群聊消息）
    QString m_currentUser;                      // 当前登录用户

    // 业务状态变量（与注册逻辑对齐）
    bool m_webSocketHandshakeDone;              // WebSocket握手完成标记（主窗口已完成，默认true）
    QString m_avatarFilePath;                   // 头像本地路径（与注册逻辑一致）
    QString m_avatarURL;                        // 头像上传后的URL（与注册逻辑一致）
    QMessageBox *m_loadingMsg;                  // 加载提示框（与注册逻辑一致）
    bool m_createGroupSuccess;                  // 群聊创建成功标记

    // 私有工具函数（与注册逻辑对齐）
    QSslConfiguration getSslConfig();           // 通用SSL配置（复刻注册逻辑，保证一致性）
    QString uploadAvatar(const QString& filePath); // 上传头像（完全复刻注册逻辑，保证可行）
    QList<QSslError> ignoreCriticalSslErrors(const QList<QSslError> &errors, QString &errorSummary); // 忽略无害SSL错误（复刻注册逻辑）
    void sendCreateGroupMessage();              // 发送群聊消息（复用主窗口SSL套接字）

    // 鼠标拖动窗口（复刻注册逻辑）
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // CREATEGROUP_H
