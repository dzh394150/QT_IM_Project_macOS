#ifndef ADDFRIENDS_H
#define ADDFRIENDS_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QSslConfiguration>
#include <QNetworkReply>
#include <QMouseEvent>
namespace Ui {
class AddFriends;
}

class AddFriends : public QWidget
{
    Q_OBJECT

public:
    explicit AddFriends(QNetworkAccessManager *netManager,
                        const QSslConfiguration &sslConf,
                        const QString &currentUser,
                        QWidget *parent = nullptr);
    ~AddFriends();

private slots:
    void onSearchFriendClicked();
    void onAddFriendClicked();
    void onSslErrors(QNetworkReply *reply, const QList<QSslError> &errors);
    void onAvatarDownloaded(QNetworkReply *reply);

protected:
    // 补充鼠标事件的声明，确保签名一致
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override; // 补充此声明

private:
    Ui::AddFriends *ui;
    QNetworkAccessManager *m_netManager;
    QSslConfiguration m_sslConf;
    QString m_currentUser;
    // 补充拖动相关成员变量的声明
    bool m_isDragging;
    QPoint m_dragStartPosition;
};

#endif // ADDFRIENDS_H
