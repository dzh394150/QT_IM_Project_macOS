#ifndef REGISTERWIDGET_H
#define REGISTERWIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QSslSocket>
#include <QNetworkAccessManager>
#include <QSslConfiguration>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class RegisterWidget; }
QT_END_NAMESPACE

class RegisterWidget : public QWidget
{
    Q_OBJECT

public:
    void handleGetRegVerifyCode();
    explicit RegisterWidget(QWidget *parent = nullptr);
    ~RegisterWidget();

protected:
    // 鼠标拖动窗口相关
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    // 核心业务槽函数
    void selectAvatar();          // 选择头像（修改版）
    void handleRegister();        // 处理注册（修改版）
    void onTcpConnected();        // TCP连接成功
    void onSslEncrypted();        // SSL握手成功
    void onSslReadyRead();        // 读取SSL数据
    void onDisconnected();        // 连接断开
    void onSocketError(QAbstractSocket::SocketError error); // 套接字错误
    void onSslErrors(const QList<QSslError> &errors);       // SSL错误

private:
    // UI对象
    Ui::RegisterWidget *ui;

    // 网络相关成员变量
    QTcpSocket *m_tcpSocket;
    QSslSocket *m_sslSocket;
    QNetworkAccessManager *m_netManager; // HTTP上传管理器（新增）
    QMessageBox *m_loadingMsg = nullptr;

    // 状态变量
    bool m_webSocketHandshakeDone;
    bool m_registerSuccess = false; // 标记注册是否成功
    QPoint m_dragPos;
    QString m_webSocketKey;

    // 头像相关（修改/新增）
    QString m_avatarBase64;       // 保留（已注释，备用）
    QString m_avatarFilePath;     // 新增：选中的头像本地路径
    QString m_avatarURL;          // 新增：上传后的头像URL

    // 核心工具函数
    void sendRegisterMessage();                     // 发送注册消息（修改版）
    QSslConfiguration getSslConfig();               // 新增：通用SSL配置函数
    QString uploadAvatar(const QString& filePath);  // 新增：HTTPS上传头像

    // 忽略两个关键错误
    QList<QSslError> ignoreCriticalSslErrors(const QList<QSslError> &errors, QString &errorSummary);
    // 【0202晚上验证码注册与登录逻辑开始：新增成员变量】
    bool m_isGetVerifyCode = false;   // 标记是否是获取验证码请求
    QString m_targetPhone;            // 目标手机号（获取验证码用）
    // 【0202晚上验证码注册与登录逻辑结束】
    void initSocket();
};

#endif // REGISTERWIDGET_H
