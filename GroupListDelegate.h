#ifndef GROUPLISTDELEGATE_H
#define GROUPLISTDELEGATE_H

#include "qabstractitemview.h"
#include <QStyledItemDelegate>
#include <QPainter>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>
#include <QHash>
#include "CacheUtils.h"
#include <QCryptographicHash>

class GroupListDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit GroupListDelegate(QObject *parent = nullptr);
    ~GroupListDelegate() override;

    // 重写绘制方法
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    // 重写项大小计算方法
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    // 【0126睡前群聊未读红点逻辑修复逻辑开始】新增更新未读数量缓存的接口
    Q_INVOKABLE void updateGroupUnreadCount(const QString &groupId, int count) {
        m_unreadCountCache[groupId] = count;
        // 收到更新后立即强制重绘（委托parent是QListView）
        if (parent()) {
            QAbstractItemView* view = qobject_cast<QAbstractItemView*>(parent());
            if (view) {
                view->viewport()->update();
            }
        }
        qDebug() << "📝 群聊委托缓存更新：" << groupId << "=" << count;
    }
    // 【0126睡前群聊未读红点逻辑修复逻辑结束】

private slots:
    // 头像下载完成槽函数
    void onAvatarDownloaded(QNetworkReply *reply);

private:
    // 下载头像（const修饰，需用mutable）
    void downloadAvatar(const QString &url) const;
    
    // 本地缓存相关方法
    QString getAvatarCachePath(const QString &url) const;
    QPixmap loadAvatarFromLocalCache(const QString &url) const;
    void saveAvatarToLocalCache(const QString &url, const QPixmap &avatar) const;

    // 成员变量（与FriendListDelegate保持一致）
    mutable QNetworkAccessManager m_netManager; // 网络管理器（mutable允许const函数修改）
    mutable QHash<QString, QPixmap> m_avatarCache; // 头像缓存（URL -> 图片）
    QString m_avatarCacheDir; // 本地头像缓存目录

    // 【0126睡前群聊未读红点逻辑修复逻辑开始】新增未读数量缓存（核心）
    QHash<QString, int> m_unreadCountCache; // 群ID→未读数量
    // 【0126睡前群聊未读红点逻辑修复逻辑结束】

    // 常量定义（与FriendListDelegate尺寸对齐）
    static const int AVATAR_SIZE = 40;    // 头像大小（和好友列表一致）
    static const int MARGIN = 5;          // 边距（和好友列表一致）
    static const int ITEM_HEIGHT = 50;    // 列表项高度（和好友列表一致）
};

#endif // GROUPLISTDELEGATE_H
