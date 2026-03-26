#include "GroupListModel.h"

GroupListModel::GroupListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

GroupListModel::~GroupListModel()
{
}

int GroupListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_groupList.size();
}

QVariant GroupListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_groupList.size())
    {
        return QVariant();
    }

    const GroupInfo &groupInfo = m_groupList.at(index.row());
    switch (role)
    {
    case GroupIdRole:
        return groupInfo.groupId;
    case GroupNameRole:
        return groupInfo.groupName;
    case GroupAvatarRole:
        return groupInfo.groupAvatar;
    case GroupMember:
        return groupInfo.members;
    // 【0123白天未读消息逻辑开始：返回群聊未读数量】
    case UnreadCountRole:
        return groupInfo.unreadCount;
    // 【0123白天未读消息逻辑结束】
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> GroupListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[GroupIdRole] = "groupId";
    roles[GroupNameRole] = "groupName";
    roles[GroupAvatarRole] = "groupAvatar";
    // 【0123白天未读消息逻辑开始：绑定未读数量角色名】
    roles[UnreadCountRole] = "unreadCount";
    // 【0123白天未读消息逻辑结束】
    return roles;
}

void GroupListModel::addGroup(const GroupInfo &groupInfo)
{
    beginInsertRows(QModelIndex(), m_groupList.size(), m_groupList.size());
    m_groupList.append(groupInfo);
    endInsertRows();
}

void GroupListModel::addGroups(const QList<GroupInfo> &groupList)
{
    if (groupList.isEmpty())
    {
        return;
    }

    beginInsertRows(QModelIndex(), m_groupList.size(), m_groupList.size() + groupList.size() - 1);
    m_groupList.append(groupList);
    endInsertRows();
}

void GroupListModel::clearGroups()
{
    beginResetModel();
    m_groupList.clear();
    endResetModel();
}

// 【0123白天未读消息逻辑开始：实现更新群聊未读数量的方法】
void GroupListModel::setGroupUnreadCount(const QString &groupId, int count)
{
    // 遍历群聊列表，找到对应群ID的项
    for (int i = 0; i < m_groupList.size(); ++i)
    {
        if (m_groupList[i].groupId == groupId)
        {
            // 更新未读数量
            m_groupList[i].unreadCount = count;
            // 触发UI更新（通知视图该行数据变化）
            QModelIndex index = createIndex(i, 0);
            emit dataChanged(index, index, {UnreadCountRole});
            break;
        }
    }
}
// 【0123白天未读消息逻辑结束】
