#include "FriendListModel.h"

FriendListModel::FriendListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    // qDebug() << "🔍 FriendListModel构造函数中的内存地址：" << this; // 打印对象地址
}

int FriendListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_friends.size();
}

QVariant FriendListModel::data(const QModelIndex &index, int role) const
{

    // qDebug() << "🔍 data函数：Model地址=" << this; // 打印对象地址
    if (!index.isValid() || index.row() >= m_friends.size()) return QVariant();

    const FriendData &data = m_friends[index.row()];
    // qDebug() << "🔢 data：行号=" << index.row() << "，用户名=" << data.username << "，未读数量=" << data.unreadCount;
    switch (role) {
    case UsernameRole: return data.username;
    case AvatarRole: return data.avatarUrl;
    // 【0123白天未读消息逻辑开始：返回未读数量】
    case UnreadCountRole:
        qDebug() << "📌 Model的data函数：行号=" << index.row()
                 << "，用户名=" << data.username
                 << "，返回未读数量=" << data.unreadCount;
        return data.unreadCount;
    // 【0123白天未读消息逻辑结束】
    default: return QVariant();
    }
}

QHash<int, QByteArray> FriendListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[UsernameRole] = "username";
    roles[AvatarRole] = "avatarUrl";
    // 【0123白天未读消息逻辑开始：绑定未读数量角色名】
    roles[UnreadCountRole] = "unreadCount";
    // 【0123白天未读消息逻辑结束】
    return roles;
}

void FriendListModel::addFriend(const FriendData &data)
{
    beginInsertRows(QModelIndex(), m_friends.size(), m_friends.size());
    m_friends.append(data);
    endInsertRows();
}

void FriendListModel::clearFriends()
{
    beginResetModel();
    m_friends.clear();
    endResetModel();
}

void FriendListModel::setFriendUnreadCount(const QString &username, int count)
{
    for (int i = 0; i < m_friends.size(); ++i) {
        if (m_friends[i].username == username) {
            m_friends[i].unreadCount = count;
            qDebug() << "✅ 最终修改：" << username << "=" << m_friends[i].unreadCount;

            // 仅发射信号，Model不处理View的更新
            QModelIndex index = createIndex(i, 0);
            emit dataChanged(index, index, {UnreadCountRole});
            break;
        }
    }
}
