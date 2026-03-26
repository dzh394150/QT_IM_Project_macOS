#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QTcpSocket>   // 新增：TCP套接字
#include <QSslSocket>   // 新增：SSL套接字
#include <QTimer>
#include <QMouseEvent>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPoint>

namespace Ui {
class LoginWidget;
}

class MainWindow;

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget() override;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    void handleLogin();
    void onTcpConnected();          // 新增：TCP连接成功
    void onSslEncrypted();          // 新增：SSL握手成功
    void onSslReadyRead();          // 新增：读取SSL数据（处理WebSocket握手/消息）
    void onDisconnected();          // 新增：连接断开
    void onSocketError(QAbstractSocket::SocketError error); // 新增：套接字错误
    void onSslErrors(const QList<QSslError> &errors);        // 新增：SSL错误
    void onWebSocketTextMessageReceived(const QString &message); // 保留原逻辑
    void reconnectSslSocketSignals();

private:
    void sendLoginMessage();        // 新增：发送登录消息（WebSocket帧）

private:
    Ui::LoginWidget *ui;
    MainWindow *m_mainWin;
    // 替换原QWebSocket：用TCP+SSL手动实现WSS
    QTcpSocket* m_tcpSocket;
    QSslSocket* m_sslSocket;
    bool m_webSocketHandshakeDone;  // 是否完成WebSocket握手
    QString m_webSocketKey;         // WebSocket握手密钥
    QTimer *m_loginTimeoutTimer;
    QPoint m_dragPos;
    QMessageBox *m_loadingMsg;
};

#endif // LOGIN_H
