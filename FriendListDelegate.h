#ifndef FRIENDLISTDELEGATE_H
#define FRIENDLISTDELEGATE_H
#include <QtWidgets/QAbstractItemView>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QPixmap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHash>

class FriendListDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit FriendListDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

    //【0126晚上未读消息无法显示修复逻辑开始】新增更新未读数量缓存的接口
    Q_INVOKABLE void updateFriendUnreadCount(const QString &username, int count) {
        m_unreadCountCache[username] = count;
        // 收到更新后立即强制重绘（委托parent是QListView）
        if (parent() && parent()->inherits("QAbstractItemView")) {
            static_cast<QAbstractItemView*>(parent())->viewport()->update();
        }
        qDebug() << "📝 委托缓存更新：" << username << "=" << count;
    }
    //【0126晚上未读消息无法显示修复逻辑结束】

private slots:
    void onAvatarDownloaded(QNetworkReply *reply);

private:
    void downloadAvatar(const QString &url) const;
    
    // 本地缓存相关方法
    QString getAvatarCachePath(const QString &url) const;
    QPixmap loadAvatarFromLocalCache(const QString &url) const;
    void saveAvatarToLocalCache(const QString &url, const QPixmap &avatar) const;

    mutable QHash<QString, QPixmap> m_avatarCache;
    mutable QNetworkAccessManager m_netManager;
    QString m_avatarCacheDir; // 本地头像缓存目录

    //【0126晚上未读消息无法显示修复逻辑开始】新增未读数量缓存（核心）
    QHash<QString, int> m_unreadCountCache; // 用户名→未读数量
    //【0126晚上未读消息无法显示修复逻辑结束】
};

#endif // FRIENDLISTDELEGATE_H
