#ifndef GROUPLISTMODEL_H
#define GROUPLISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QString>

// 群聊信息结构体
struct GroupInfo
{
    QString groupId;       // 群ID
    QString groupName;     // 群名称
    QString groupAvatar;   // 群头像路径（可根据实际需求调整为二进制数据等）
    QString members;       // 群成员
    // 【0123白天未读消息逻辑开始：新增群聊未读数量字段】
    int unreadCount = 0;   // 群聊未读消息数量，默认0
    // 【0123白天未读消息逻辑结束】
};

class GroupListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    // 自定义角色，用于获取对应数据
    enum GroupRole
    {
        GroupIdRole = Qt::UserRole + 1,
        GroupNameRole = Qt::UserRole + 2,
        GroupAvatarRole,
        GroupMember,
        // 【0123白天未读消息逻辑开始：新增群聊未读数量角色】
        UnreadCountRole     // 群聊未读数量角色
        // 【0123白天未读消息逻辑结束】
    };

    explicit GroupListModel(QObject *parent = nullptr);
    ~GroupListModel() override;

    // 重写QAbstractListModel虚函数
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // 公共方法：添加单个群聊、批量添加群聊、清空群聊列表
    void addGroup(const GroupInfo &groupInfo);
    void addGroups(const QList<GroupInfo> &groupList);
    void clearGroups();

    // 【0123白天未读消息逻辑开始：新增更新群聊未读数量的方法】
    void setGroupUnreadCount(const QString &groupId, int count);
    // 【0123白天未读消息逻辑结束】

private:
    QList<GroupInfo> m_groupList;  // 存储所有群聊信息的列表
};

#endif // GROUPLISTMODEL_H
