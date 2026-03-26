// chatmodel.cpp
#include "chatmodel.h"

// 【0113晚上新增功能：补充static const成员的类外定义，解决未定义引用错误】
const int ChatModel::FileAttachRole; // 无需重复赋值，头文件中已初始化
ChatModel::ChatModel(QObject *parent)
    : QAbstractListModel(parent)
{}

int ChatModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_messages.size();
}

// 2. 修改data方法（支持返回FileAttachment数据）
QVariant ChatModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_messages.size())
        return QVariant();

    const ChatMessage &msg = m_messages[index.row()];
    switch (role) {
    case Qt::DisplayRole:
        return msg.content;
    case Qt::UserRole + 1: // OwnerRole
        return msg.owner;
    case Qt::UserRole + 2: // TypeRole
        return msg.type;
    case Qt::UserRole + 3: // SenderNameRole
        return msg.senderName;
    case Qt::UserRole + 4: // AvatarPathRole
        return msg.avatarPath;
    case Qt::UserRole + 5: // TimestampRole
        return msg.timestamp.toString("yyyy-MM-dd hh:mm:ss");
    case FileAttachRole: // 【0113晚上新增功能：返回文件附件数据】
        return QVariant::fromValue(msg.fileAttach);
    default:
        return QVariant();
    }
}

// 1. 修改roleNames方法（新增FileAttachRole）
QHash<int, QByteArray> ChatModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "content";
    roles[Qt::UserRole + 1] = "owner";
    roles[Qt::UserRole + 2] = "type";
    roles[Qt::UserRole + 3] = "senderName";
    roles[Qt::UserRole + 4] = "avatarPath";
    roles[Qt::UserRole + 5] = "timestamp";
    // 【0113晚上新增功能：新增文件附件角色，用于传递FileAttachment数据】
    roles[FileAttachRole] = "fileAttach";
    return roles;
}

void ChatModel::addMessage(const ChatMessage &msg)
{
    beginInsertRows(QModelIndex(), m_messages.size(), m_messages.size());
    m_messages.append(msg);
    endInsertRows();
}

void ChatModel::prependMessages(const QVector<ChatMessage> &messages)
{
    if (messages.isEmpty())
        return;

    beginInsertRows(QModelIndex(), 0, messages.size() - 1);
    // 为了保持顺序，需要从后往前插入消息
    for (auto it = messages.rbegin(); it != messages.rend(); ++it) {
        m_messages.prepend(*it);
    }
    endInsertRows();
}

void ChatModel::removeLastMessage()
{
    if (m_messages.isEmpty())
        return;

    beginRemoveRows(QModelIndex(), m_messages.size() - 1, m_messages.size() - 1);
    m_messages.removeLast();
    endRemoveRows();
}

void ChatModel::clearMessages()
{
    beginResetModel();
    m_messages.clear();
    m_hasMoreMessages = true; // 重置是否有更多消息的标记
    endResetModel();
}
