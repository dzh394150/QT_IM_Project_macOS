#ifndef FRIENDLISTMODEL_H
#define FRIENDLISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QString>

// 仅在模型头文件中定义FriendData（避免重复）
struct FriendData {
    QString username;
    QString avatarUrl;
    // 【0123白天未读消息逻辑开始：新增未读数量字段】
    int unreadCount = 0; // 好友未读消息数量，默认0
    // 【0123白天未读消息逻辑结束】
};

class FriendListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    // 定义角色枚举（供QListView绑定）
    enum FriendRoles {
        UsernameRole = Qt::UserRole + 1,
        AvatarRole = Qt::UserRole + 2,
        // 【0123白天未读消息逻辑开始：新增未读数量角色】
        UnreadCountRole = Qt::UserRole + 3 // 未读数量角色
        // 【0123白天未读消息逻辑结束】
    };

    // ========== 新增：非const接口，直接读取最新值 ==========
    int getFriendUnreadCountDirectly(const QString &username) {
        // 非const函数，直接访问m_friends，无隐式拷贝
        for (int i = 0; i < m_friends.size(); ++i) {
            if (m_friends[i].username == username) {
                qDebug() << "🔥 直接读取未读数量：" << username << "=" << m_friends[i].unreadCount;
                return m_friends[i].unreadCount;
            }
        }
        return 0;
    }

    explicit FriendListModel(QObject *parent = nullptr);

    // 重写QAbstractListModel方法
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // 自定义方法
    void addFriend(const FriendData &data);
    void clearFriends();
    // 【0123白天未读消息逻辑开始：新增更新好友未读数量的方法】
    void setFriendUnreadCount(const QString &username, int count);
    // 【0123白天未读消息逻辑结束】

private:
    // 把QList改为QVector（唯一修改）
    QVector<FriendData> m_friends;
};

#endif // FRIENDLISTMODEL_H
