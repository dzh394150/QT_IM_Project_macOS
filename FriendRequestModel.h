#ifndef FRIENDREQUESTMODEL_H
#define FRIENDREQUESTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QString>

// 仅在模型头文件中定义FriendRequestData（避免重复）
struct FriendRequestData {
    QString fromUser;
    QString avatarUrl;
    QString message;
    int status; // 0:待处理, 1:已同意, 2:已拒绝
};

class FriendRequestModel : public QAbstractListModel
{
    Q_OBJECT
public:
    // 定义角色枚举（包含FromUserRole）
    enum RequestRoles {
        FromUserRole = Qt::UserRole + 1,
        AvatarUrlRole,
        MessageRole,
        StatusRole // 新增：状态角色，用于获取处理状态
    };

    explicit FriendRequestModel(QObject *parent = nullptr);

    // 重写QAbstractListModel方法
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // 自定义方法
    void addRequest(const FriendRequestData &data);
    void clearRequests();
    FriendRequestData getRequest(int row) const;
private:
    QList<FriendRequestData> m_requests;
};

#endif // FRIENDREQUESTMODEL_H
