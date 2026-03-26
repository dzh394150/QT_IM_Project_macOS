#ifndef FRIENDREQUESTDELEGATE_H
#define FRIENDREQUESTDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QPixmap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHash>
#include <QtWidgets/QAbstractItemView>
#include "CacheUtils.h"
#include <QCryptographicHash>
class FriendRequestDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit FriendRequestDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

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
};

#endif // FRIENDREQUESTDELEGATE_H
