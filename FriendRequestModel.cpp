#include "FriendRequestModel.h"
FriendRequestModel::FriendRequestModel(QObject *parent)
    : QAbstractListModel(parent)
{}

int FriendRequestModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_requests.size();
}

QVariant FriendRequestModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_requests.size()) return QVariant();

    const FriendRequestData &data = m_requests[index.row()];
    switch (role) {
    case FromUserRole: return data.fromUser;
    case AvatarUrlRole: return data.avatarUrl;
    case MessageRole: return data.message;
    case StatusRole: return data.status; // 新增：返回处理状态
    default: return QVariant();
    }
}

QHash<int, QByteArray> FriendRequestModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[FromUserRole] = "fromUser";
    roles[AvatarUrlRole] = "avatarUrl";
    roles[MessageRole] = "message";
    roles[StatusRole] = "status"; // 新增：状态角色映射
    return roles;
}

void FriendRequestModel::addRequest(const FriendRequestData &data)
{
    qDebug() << "📩 [FriendRequestModel::addRequest] 添加好友请求（置顶）："
             << "fromUser=" << data.fromUser
             << "avatarUrl=" << data.avatarUrl
             << "message=" << data.message
             << "status=" << data.status;

    // 核心修改：插入到列表头部（置顶），而非追加到尾部
    beginInsertRows(QModelIndex(), 0, 0); // 插入位置改为0（第一行）
    m_requests.prepend(data); // prepend替代append，插入头部
    endInsertRows();

    qDebug() << "📊 [FriendRequestModel::addRequest] 当前请求数：" << m_requests.size();
}

void FriendRequestModel::clearRequests()
{
    beginResetModel();
    m_requests.clear();
    endResetModel();
}

FriendRequestData FriendRequestModel::getRequest(int row) const {
    if (row >= 0 && row < m_requests.size()) {
        return m_requests[row];
    }
    return FriendRequestData();
}
