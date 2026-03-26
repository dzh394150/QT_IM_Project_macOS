#ifndef LOCALCHATCACHE_H
#define LOCALCHATCACHE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVector>
#include <QString>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include <QJsonDocument>
#include <QJsonObject>

// 引入必要的结构体
struct ChatMessage;
#include "FriendListModel.h"
#include "GroupListModel.h"

/**
 * @brief 本地聊天缓存管理类
 * 
 * 该类使用单例模式实现，负责本地聊天记录的缓存管理，包括：
 * - 本地数据库初始化
 * - 聊天记录的查询、保存、更新
 * - 同步状态管理
 * - 增量同步支持
 */
class LocalChatCache : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 获取单例实例
     * @return LocalChatCache* 单例指针
     */
    static LocalChatCache* instance();

    /**
     * @brief 初始化本地缓存
     * @param username 当前用户名
     * @return bool 初始化是否成功
     */
    bool init(const QString &username);

    /**
     * @brief 获取聊天记录
     * @param fromUser 发送者用户名
     * @param toUser 接收者用户名/群ID
     * @param limit 获取数量限制
     * @param beforeTs 时间戳，获取该时间之前的记录
     * @return QVector<ChatMessage> 聊天记录列表
     */
    QVector<ChatMessage> getChatRecords(const QString &fromUser, const QString &toUser, int limit = 20, qint64 beforeTs = 0);

    /**
     * @brief 保存单条消息
     * @param message 聊天消息
     * @param isSynced 是否已同步到云端
     * @return bool 保存是否成功
     */
    bool saveMessage(const ChatMessage &message, bool isSynced = false);

    /**
     * @brief 批量保存消息
     * @param messages 聊天消息列表
     * @param isSynced 是否已同步到云端
     * @return bool 保存是否成功
     */
    bool saveMessages(const QVector<ChatMessage> &messages, bool isSynced = false);

    /**
     * @brief 更新消息同步状态
     * @param msgTs 消息时间戳
     * @param isSynced 是否已同步到云端
     * @return bool 更新是否成功
     */
    bool updateMessageSyncStatus(qint64 msgTs, bool isSynced);

    /**
     * @brief 标记消息为已读
     * @param fromUser 发送者用户名
     * @param toUser 接收者用户名/群ID
     * @return bool 标记是否成功
     */
    bool markMessagesAsRead(const QString &fromUser, const QString &toUser);

    /**
     * @brief 获取未同步的消息
     * @return QVector<ChatMessage> 未同步的消息列表
     */
    QVector<ChatMessage> getUnsyncedMessages();

    /**
     * @brief 检查是否需要与云端同步
     * @param fromUser 发送者用户名
     * @param toUser 接收者用户名/群ID
     * @return bool 是否需要同步
     */
    bool needSyncWithCloud(const QString &fromUser, const QString &toUser);

    /**
     * @brief 关闭本地缓存
     */
    void close();

    /**
     * @brief 获取用户信息
     * @param username 用户名
     * @return FriendData 用户信息
     */
    FriendData getUserInfo(const QString &username);

    /**
     * @brief 保存用户信息
     * @param userInfo 用户信息
     * @return bool 保存是否成功
     */
    bool saveUserInfo(const FriendData &userInfo);

    /**
     * @brief 批量保存用户信息
     * @param userInfos 用户信息列表
     * @return bool 保存是否成功
     */
    bool saveUserInfos(const QList<FriendData> &userInfos);

    /**
     * @brief 获取所有用户信息
     * @return QList<FriendData> 用户信息列表
     */
    QList<FriendData> getAllUserInfos();

    /**
     * @brief 获取群聊信息
     * @param groupId 群ID
     * @return GroupInfo 群聊信息
     */
    GroupInfo getGroupInfo(const QString &groupId);

    /**
     * @brief 保存群聊信息
     * @param groupInfo 群聊信息
     * @return bool 保存是否成功
     */
    bool saveGroupInfo(const GroupInfo &groupInfo);

    /**
     * @brief 批量保存群聊信息
     * @param groupInfos 群聊信息列表
     * @return bool 保存是否成功
     */
    bool saveGroupInfos(const QList<GroupInfo> &groupInfos);

    /**
     * @brief 获取所有群聊信息
     * @return QList<GroupInfo> 群聊信息列表
     */
    QList<GroupInfo> getAllGroupInfos();

    /**
     * @brief 获取用户头像URL
     * @param username 用户名
     * @return QString 头像URL，如果不存在返回空字符串
     */
    QString getUserAvatarUrl(const QString &username);

    /**
     * @brief 获取群聊头像URL
     * @param groupId 群ID
     * @return QString 头像URL，如果不存在返回空字符串
     */
    QString getGroupAvatarUrl(const QString &groupId);

signals:
    /**
     * @brief 同步完成信号
     * @param success 同步是否成功
     */
    void syncCompleted(bool success);

    /**
     * @brief 有新消息可用信号
     * @param fromUser 发送者用户名
     * @param toUser 接收者用户名/群ID
     */
    void newMessagesAvailable(const QString &fromUser, const QString &toUser);

private:
    /**
     * @brief 构造函数（私有，单例模式）
     * @param parent 父对象
     */
    explicit LocalChatCache(QObject *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~LocalChatCache() override;

    /**
     * @brief 创建数据库表
     * @return bool 创建是否成功
     */
    bool createTables();

    /**
     * @brief 查询本地聊天记录
     * @param fromUser 发送者用户名
     * @param toUser 接收者用户名/群ID
     * @param limit 获取数量限制
     * @param beforeTs 时间戳，获取该时间之前的记录
     * @return QVector<ChatMessage> 聊天记录列表
     */
    QVector<ChatMessage> queryLocalChatRecords(const QString &fromUser, const QString &toUser, int limit, qint64 beforeTs);

    /**
     * @brief 执行SQL查询
     * @param query SQL查询语句
     * @param sql SQL语句（可选，如果已prepare则可不传）
     * @return bool 执行是否成功
     */
    bool executeQuery(QSqlQuery &query, const QString &sql = QString());

    // 单例实例
    static LocalChatCache* m_instance;

    // 数据库连接
    QSqlDatabase m_db;

    // 数据库是否初始化成功
    bool m_isInitialized;

    // 当前用户名
    QString m_username;

    // 数据库文件路径
    QString m_dbPath;
};

#endif // LOCALCHATCACHE_H