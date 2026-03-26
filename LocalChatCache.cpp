#include "LocalChatCache.h"
#include "chatmessage.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDir>

// 初始化单例实例
LocalChatCache* LocalChatCache::m_instance = nullptr;

/**
 * @brief 获取单例实例
 * 线程安全的懒汉式单例实现
 */
LocalChatCache* LocalChatCache::instance()
{
    if (!m_instance) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!m_instance) {
            m_instance = new LocalChatCache();
        }
    }
    return m_instance;
}

/**
 * @brief 构造函数
 * 私有构造函数，确保只能通过instance()获取实例
 */
LocalChatCache::LocalChatCache(QObject *parent) : QObject(parent)
    , m_isInitialized(false)
{
    // 初始化数据库连接
    m_db = QSqlDatabase::addDatabase("QSQLITE");
}

/**
 * @brief 析构函数
 * 关闭数据库连接
 */
LocalChatCache::~LocalChatCache()
{
    close();
    m_instance = nullptr;
}

/**
 * @brief 初始化本地缓存
 * @param username 当前用户名，用于区分不同用户的缓存
 * @return 是否初始化成功
 */
bool LocalChatCache::init(const QString &username)
{
    if (m_isInitialized) {
        // 如果已经初始化且用户名相同，则直接返回成功
        if (m_username == username) {
            return true;
        } else {
            // 如果用户名不同，先关闭当前数据库
            close();
        }
    }

    m_username = username;

    // 获取应用数据目录
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (dataDir.isEmpty()) {
        qCritical() << "无法获取应用数据目录";
        return false;
    }

    // 创建目录（如果不存在）
    QDir dir(dataDir);
    if (!dir.exists() && !dir.mkpath(".")) {
        qCritical() << "无法创建应用数据目录:" << dataDir;
        return false;
    }

    // 数据库文件路径：应用数据目录/用户名_chat_cache.db
    m_dbPath = dir.filePath(username + "_chat_cache.db");
    m_db.setDatabaseName(m_dbPath);

    // 打开数据库
    if (!m_db.open()) {
        qCritical() << "无法打开数据库:" << m_db.lastError().text();
        return false;
    }

    // 创建数据表
    if (!createTables()) {
        m_db.close();
        return false;
    }

    m_isInitialized = true;
    qDebug() << "本地聊天缓存初始化成功，数据库路径:" << m_dbPath;
    return true;
}

/**
 * @brief 创建数据库表结构
 * 包括聊天记录表和同步状态表
 * @return 是否创建成功
 */
bool LocalChatCache::createTables()
{
    QSqlQuery query(m_db);

    // 0. 创建用户信息表
    // 字段说明：
    // - username: 用户名，主键
    // - avatar_url: 头像URL
    // - unread_count: 未读消息数量
    // - last_updated: 最后更新时间戳（毫秒级）
    QString createUsersTableSql = R"(
        CREATE TABLE IF NOT EXISTS users (
            username TEXT PRIMARY KEY,
            avatar_url TEXT,
            unread_count INTEGER DEFAULT 0,
            last_updated INTEGER NOT NULL
        );
    )";

    if (!executeQuery(query, createUsersTableSql)) {
        qCritical() << "创建用户信息表失败:" << query.lastError().text();
        return false;
    }

    // 0.1 创建群聊信息表
    // 字段说明：
    // - group_id: 群ID，主键
    // - group_name: 群名称
    // - group_avatar: 群头像URL
    // - members: 群成员
    // - unread_count: 未读消息数量
    // - last_updated: 最后更新时间戳（毫秒级）
    QString createGroupsTableSql = R"(
        CREATE TABLE IF NOT EXISTS groups (
            group_id TEXT PRIMARY KEY,
            group_name TEXT NOT NULL,
            group_avatar TEXT,
            members TEXT,
            unread_count INTEGER DEFAULT 0,
            last_updated INTEGER NOT NULL
        );
    )";

    if (!executeQuery(query, createGroupsTableSql)) {
        qCritical() << "创建群聊信息表失败:" << query.lastError().text();
        return false;
    }

    // 1. 创建聊天记录表
    // 字段说明：
    // - id: 自增主键
    // - from_user: 发送者用户名
    // - to_user: 接收者用户名/群ID
    // - content: 消息内容
    // - type: 消息类型（文本/图片/文件）
    // - timestamp: 消息时间戳（毫秒级）
    // - is_read: 是否已读
    // - is_synced: 是否已同步到云端
    // - sender_name: 发送者昵称
    // - avatar_path: 头像路径
    // - file_info: 文件信息（JSON格式）
    QString createChatTableSql = R"(
        CREATE TABLE IF NOT EXISTS chat_records (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            from_user TEXT NOT NULL,
            to_user TEXT NOT NULL,
            content TEXT NOT NULL,
            type INTEGER NOT NULL,
            timestamp INTEGER NOT NULL,
            is_read BOOLEAN DEFAULT 0,
            is_synced BOOLEAN DEFAULT 0,
            sender_name TEXT NOT NULL,
            avatar_path TEXT,
            file_info TEXT,
            UNIQUE(from_user, to_user, timestamp)
        );
    )";

    if (!executeQuery(query, createChatTableSql)) {
        qCritical() << "创建聊天记录表失败:" << query.lastError().text();
        return false;
    }

    // 2. 创建索引，提高查询效率（分开执行每条SQL语句）
    QStringList indexSqlList = {
        "CREATE INDEX IF NOT EXISTS idx_chat_records_from_to ON chat_records(from_user, to_user);",
        "CREATE INDEX IF NOT EXISTS idx_chat_records_timestamp ON chat_records(timestamp);",
        "CREATE INDEX IF NOT EXISTS idx_chat_records_is_synced ON chat_records(is_synced);"
    };

    for (const QString &indexSql : indexSqlList) {
        if (!executeQuery(query, indexSql)) {
            qCritical() << "创建索引失败:" << query.lastError().text() << " SQL:" << indexSql;
            return false;
        }
    }

    // 3. 创建同步状态表
    // 记录每个会话的最后同步时间
    QString createSyncTableSql = R"(
        CREATE TABLE IF NOT EXISTS sync_status (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            from_user TEXT NOT NULL,
            to_user TEXT NOT NULL,
            last_sync_timestamp INTEGER DEFAULT 0,
            UNIQUE(from_user, to_user)
        );
    )";

    if (!executeQuery(query, createSyncTableSql)) {
        qCritical() << "创建同步状态表失败:" << query.lastError().text();
        return false;
    }

    return true;
}

/**
 * @brief 获取聊天记录
 * 先从本地数据库查询，不足时由调用者从云端获取
 * @param fromUser 发送者
 * @param toUser 接收者/群ID
 * @param limit 最大获取数量
 * @param beforeTs 只获取该时间戳之前的记录（0表示获取最新的）
 * @return 聊天记录列表
 */
QVector<ChatMessage> LocalChatCache::getChatRecords(const QString &fromUser, const QString &toUser, int limit, qint64 beforeTs)
{
    if (!m_isInitialized) {
        qWarning() << "本地缓存未初始化，无法获取聊天记录";
        return QVector<ChatMessage>();
    }

    return queryLocalChatRecords(fromUser, toUser, limit, beforeTs);
}

/**
 * @brief 查询本地聊天记录
 * 实际执行数据库查询的内部函数
 */
QVector<ChatMessage> LocalChatCache::queryLocalChatRecords(const QString &fromUser, const QString &toUser, int limit, qint64 beforeTs)
{
    QVector<ChatMessage> messages;
    QSqlQuery query(m_db);

    QString sql = R"(
        SELECT content, type, timestamp, is_read, sender_name, avatar_path, file_info 
        FROM chat_records 
        WHERE (from_user = :from AND to_user = :to) OR (from_user = :to AND to_user = :from) 
    )";

    // 添加时间戳条件
    if (beforeTs > 0) {
        sql += " AND timestamp < :beforeTs ";
    }

    // 按时间戳降序排列，取最新的记录
    sql += " ORDER BY timestamp DESC LIMIT :limit";

    query.prepare(sql);
    query.bindValue(":from", fromUser);
    query.bindValue(":to", toUser);
    if (beforeTs > 0) {
        query.bindValue(":beforeTs", beforeTs);
    }
    query.bindValue(":limit", limit);

    if (!executeQuery(query)) {
        qWarning() << "查询聊天记录失败:" << query.lastError().text();
        return messages;
    }

    // 解析查询结果
    while (query.next()) {
        ChatMessage msg;
        msg.content = query.value(0).toString();
        msg.type = static_cast<ChatMessage::MessageType>(query.value(1).toInt());
        msg.timestamp = QDateTime::fromMSecsSinceEpoch(query.value(2).toLongLong());
        msg.isRead = query.value(3).toBool();
        msg.senderName = query.value(4).toString();
        msg.avatarPath = query.value(5).toString();

        // 解析文件信息
        QString fileInfoJson = query.value(6).toString();
        if (!fileInfoJson.isEmpty()) {
            QJsonDocument jsonDoc = QJsonDocument::fromJson(fileInfoJson.toUtf8());
            if (jsonDoc.isObject()) {
                QJsonObject jsonObj = jsonDoc.object();
                msg.fileAttach.downloadUrl = jsonObj["download_url"].toString();
                msg.fileAttach.originUrl = jsonObj["origin_url"].toString();
                msg.fileAttach.previewUrl = jsonObj["preview_url"].toString();
                msg.fileAttach.fileName = jsonObj["file_name"].toString();
                msg.fileAttach.fileSuffix = jsonObj["file_suffix"].toString();
                msg.fileAttach.isImageFile = jsonObj["is_image_file"].toBool();
            }
        }

        // 确定消息所有者
        msg.owner = (msg.senderName == m_username) ? ChatMessage::Self : ChatMessage::Other;

        messages.append(msg);
    }

    // 注意：查询结果是按时间降序排列的，需要反转以保持时间正序
    std::reverse(messages.begin(), messages.end());

    return messages;
}

/**
 * @brief 保存单条消息
 * @param message 要保存的消息
 * @param isSynced 是否已同步到云端
 * @return 是否保存成功
 */
bool LocalChatCache::saveMessage(const ChatMessage &message, bool isSynced)
{
    if (!m_isInitialized) {
        qWarning() << "本地缓存未初始化，无法保存消息";
        return false;
    }

    QSqlQuery query(m_db);
    QString sql = R"(
        INSERT OR REPLACE INTO chat_records (
            from_user, to_user, content, type, timestamp, is_read, is_synced, sender_name, avatar_path, file_info
        ) VALUES (:from, :to, :content, :type, :timestamp, :is_read, :is_synced, :sender_name, :avatar_path, :file_info)
    )";

    query.prepare(sql);
    query.bindValue(":from", message.senderName);
    query.bindValue(":to", message.receiverName);
    query.bindValue(":content", message.content);
    query.bindValue(":type", message.type);
    query.bindValue(":timestamp", message.timestamp.toMSecsSinceEpoch());
    query.bindValue(":is_read", message.isRead);
    query.bindValue(":is_synced", isSynced);
    query.bindValue(":sender_name", message.senderName);
    query.bindValue(":avatar_path", message.avatarPath);

    // 序列化文件信息为JSON
    QJsonObject fileInfoObj;
    if (message.type == ChatMessage::Image || message.type == ChatMessage::File) {
        fileInfoObj["download_url"] = message.fileAttach.downloadUrl;
        fileInfoObj["origin_url"] = message.fileAttach.originUrl;
        fileInfoObj["preview_url"] = message.fileAttach.previewUrl;
        fileInfoObj["file_name"] = message.fileAttach.fileName;
        fileInfoObj["file_suffix"] = message.fileAttach.fileSuffix;
        fileInfoObj["is_image_file"] = message.fileAttach.isImageFile;
    }
    query.bindValue(":file_info", QJsonDocument(fileInfoObj).toJson(QJsonDocument::Compact));

    if (!executeQuery(query)) {
        qWarning() << "保存消息失败:" << query.lastError().text();
        return false;
    }

    // 发送新消息可用信号
    emit newMessagesAvailable(message.senderName, message.receiverName);

    return true;
}

/**
 * @brief 批量保存消息
 * 使用事务提高性能
 * @param messages 消息列表
 * @param isSynced 是否已同步到云端
 * @return 是否保存成功
 */
bool LocalChatCache::saveMessages(const QVector<ChatMessage> &messages, bool isSynced)
{
    if (!m_isInitialized || messages.isEmpty()) {
        return false;
    }

    // 开始事务
    if (!m_db.transaction()) {
        qCritical() << "无法开始事务:" << m_db.lastError().text();
        return false;
    }

    bool allSuccess = true;
    QSqlQuery query(m_db);
    QString sql = R"(
        INSERT OR REPLACE INTO chat_records (
            from_user, to_user, content, type, timestamp, is_read, is_synced, sender_name, avatar_path, file_info
        ) VALUES (:from, :to, :content, :type, :timestamp, :is_read, :is_synced, :sender_name, :avatar_path, :file_info)
    )";

    query.prepare(sql);

    for (const auto &message : messages) {
        query.bindValue(":from", message.senderName);
        query.bindValue(":to", message.receiverName);
        query.bindValue(":content", message.content);
        query.bindValue(":type", message.type);
        query.bindValue(":timestamp", message.timestamp.toMSecsSinceEpoch());
        query.bindValue(":is_read", message.isRead);
        query.bindValue(":is_synced", isSynced);
        query.bindValue(":sender_name", message.senderName);
        query.bindValue(":avatar_path", message.avatarPath);

        // 序列化文件信息
        QJsonObject fileInfoObj;
        if (message.type == ChatMessage::Image || message.type == ChatMessage::File) {
            fileInfoObj["download_url"] = message.fileAttach.downloadUrl;
            fileInfoObj["origin_url"] = message.fileAttach.originUrl;
            fileInfoObj["preview_url"] = message.fileAttach.previewUrl;
            fileInfoObj["file_name"] = message.fileAttach.fileName;
            fileInfoObj["file_suffix"] = message.fileAttach.fileSuffix;
            fileInfoObj["is_image_file"] = message.fileAttach.isImageFile;
        }
        query.bindValue(":file_info", QJsonDocument(fileInfoObj).toJson(QJsonDocument::Compact));

        if (!executeQuery(query)) {
            qWarning() << "批量保存消息失败:" << query.lastError().text();
            allSuccess = false;
            break;
        }
        
        // 重置查询对象，准备下一次参数绑定
        query.clear();
        query.prepare(sql);
    }

    // 提交或回滚事务
    if (allSuccess) {
        if (!m_db.commit()) {
            qCritical() << "提交事务失败:" << m_db.lastError().text();
            m_db.rollback();
            return false;
        }
        // 发送新消息可用信号
        if (!messages.isEmpty()) {
            const auto &firstMsg = messages.first();
            emit newMessagesAvailable(firstMsg.senderName, firstMsg.receiverName);
        }
    } else {
        m_db.rollback();
    }

    return allSuccess;
}

/**
 * @brief 更新消息同步状态
 * @param msgTs 消息时间戳（唯一标识）
 * @param isSynced 新的同步状态
 * @return 是否更新成功
 */
bool LocalChatCache::updateMessageSyncStatus(qint64 msgTs, bool isSynced)
{
    if (!m_isInitialized) {
        qWarning() << "本地缓存未初始化，无法更新同步状态";
        return false;
    }

    QSqlQuery query(m_db);
    QString sql = "UPDATE chat_records SET is_synced = :is_synced WHERE timestamp = :timestamp";
    query.prepare(sql);
    query.bindValue(":is_synced", isSynced);
    query.bindValue(":timestamp", msgTs);

    if (!executeQuery(query)) {
        qWarning() << "更新同步状态失败:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

/**
 * @brief 标记消息为已读
 * @param fromUser 发送者
 * @param toUser 接收者/群ID
 * @return 是否标记成功
 */
bool LocalChatCache::markMessagesAsRead(const QString &fromUser, const QString &toUser)
{
    if (!m_isInitialized) {
        qWarning() << "本地缓存未初始化，无法标记已读";
        return false;
    }

    QSqlQuery query(m_db);
    QString sql = R"(
        UPDATE chat_records SET is_read = 1 
        WHERE from_user = :from AND to_user = :to AND is_read = 0
    )";
    query.prepare(sql);
    query.bindValue(":from", fromUser);
    query.bindValue(":to", toUser);

    if (!executeQuery(query)) {
        qWarning() << "标记已读失败:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

/**
 * @brief 获取未同步的消息
 * 用于同步本地消息到云端
 * @return 未同步的消息列表
 */
QVector<ChatMessage> LocalChatCache::getUnsyncedMessages()
{
    QVector<ChatMessage> messages;
    if (!m_isInitialized) {
        qWarning() << "本地缓存未初始化，无法获取未同步消息";
        return messages;
    }

    QSqlQuery query(m_db);
    QString sql = R"(
        SELECT content, type, timestamp, sender_name, receiver_name, avatar_path, file_info 
        FROM chat_records 
        WHERE is_synced = 0 
        ORDER BY timestamp ASC
    )";

    if (!executeQuery(query)) {
        qWarning() << "查询未同步消息失败:" << query.lastError().text();
        return messages;
    }

    // 解析查询结果
    while (query.next()) {
        ChatMessage msg;
        msg.content = query.value(0).toString();
        msg.type = static_cast<ChatMessage::MessageType>(query.value(1).toInt());
        msg.timestamp = QDateTime::fromMSecsSinceEpoch(query.value(2).toLongLong());
        msg.senderName = query.value(3).toString();
        msg.receiverName = query.value(4).toString();
        msg.avatarPath = query.value(5).toString();
        msg.isSynced = false;
        msg.isRead = true; // 未同步的消息都是自己发送的，默认为已读
        msg.owner = ChatMessage::Self;

        // 解析文件信息
        QString fileInfoJson = query.value(6).toString();
        if (!fileInfoJson.isEmpty()) {
            QJsonDocument jsonDoc = QJsonDocument::fromJson(fileInfoJson.toUtf8());
            if (jsonDoc.isObject()) {
                QJsonObject jsonObj = jsonDoc.object();
                msg.fileAttach.downloadUrl = jsonObj["download_url"].toString();
                msg.fileAttach.originUrl = jsonObj["origin_url"].toString();
                msg.fileAttach.previewUrl = jsonObj["preview_url"].toString();
                msg.fileAttach.fileName = jsonObj["file_name"].toString();
                msg.fileAttach.fileSuffix = jsonObj["file_suffix"].toString();
                msg.fileAttach.isImageFile = jsonObj["is_image_file"].toBool();
            }
        }

        messages.append(msg);
    }

    return messages;
}

/**
 * @brief 检查是否需要与云端同步
 * 通过比较本地最后同步时间和云端最新时间判断
 * @param fromUser 发送者
 * @param toUser 接收者/群ID
 * @return 是否需要同步
 */
bool LocalChatCache::needSyncWithCloud(const QString &fromUser, const QString &toUser)
{
    if (!m_isInitialized) {
        qWarning() << "本地缓存未初始化，无法检查同步状态";
        return true; // 未初始化时默认需要同步
    }

    // 这里简化实现，实际项目中应该与云端最新时间戳比较
    QSqlQuery query(m_db);
    QString sql = R"(
        SELECT last_sync_timestamp FROM sync_status 
        WHERE from_user = :from AND to_user = :to
    )";
    query.prepare(sql);
    query.bindValue(":from", fromUser);
    query.bindValue(":to", toUser);

    if (!executeQuery(query)) {
        qWarning() << "查询同步状态失败:" << query.lastError().text();
        return true;
    }

    // 如果没有同步记录，需要同步
    if (!query.next()) {
        return true;
    }

    // 获取本地最后同步时间
    qint64 lastSyncTs = query.value(0).toLongLong();

    // 获取本地最新消息时间
    QSqlQuery maxTsQuery(m_db);
    sql = R"(
        SELECT MAX(timestamp) FROM chat_records 
        WHERE (from_user = :from AND to_user = :to) OR (from_user = :to AND to_user = :from)
    )";
    maxTsQuery.prepare(sql);
    maxTsQuery.bindValue(":from", fromUser);
    maxTsQuery.bindValue(":to", toUser);

    if (!executeQuery(maxTsQuery)) {
        qWarning() << "查询本地最新消息时间失败:" << maxTsQuery.lastError().text();
        return true;
    }

    qint64 localMaxTs = maxTsQuery.next() ? maxTsQuery.value(0).toLongLong() : 0;

    // 如果本地有比最后同步时间更新的消息，需要同步
    return localMaxTs > lastSyncTs;
}

// ========== 用户信息缓存方法实现 ==========
FriendData LocalChatCache::getUserInfo(const QString &username) {
    if (!m_isInitialized) {
        qWarning() << "[LocalChatCache::getUserInfo] 缓存未初始化";
        return FriendData();
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT avatar_url, unread_count FROM users WHERE username = ?");
    query.addBindValue(username);

    if (!executeQuery(query)) {
        return FriendData();
    }

    FriendData userInfo;
    userInfo.username = username;
    if (query.next()) {
        userInfo.avatarUrl = query.value(0).toString();
        userInfo.unreadCount = query.value(1).toInt();
    }

    return userInfo;
}

bool LocalChatCache::saveUserInfo(const FriendData &userInfo) {
    if (!m_isInitialized) {
        qWarning() << "[LocalChatCache::saveUserInfo] 缓存未初始化";
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("INSERT OR REPLACE INTO users(username, avatar_url, unread_count, last_updated) VALUES(?, ?, ?, ?)");
    query.addBindValue(userInfo.username);
    query.addBindValue(userInfo.avatarUrl);
    query.addBindValue(userInfo.unreadCount);
    query.addBindValue(QDateTime::currentMSecsSinceEpoch());

    return executeQuery(query);
}

bool LocalChatCache::saveUserInfos(const QList<FriendData> &userInfos) {
    if (!m_isInitialized || userInfos.isEmpty()) {
        qWarning() << "[LocalChatCache::saveUserInfos] 缓存未初始化或用户信息列表为空";
        return false;
    }

    if (!m_db.transaction()) {
        qWarning() << "[LocalChatCache::saveUserInfos] 开启事务失败:" << m_db.lastError().text();
        return false;
    }

    QSqlQuery query(m_db);
    QString sql = "INSERT OR REPLACE INTO users(username, avatar_url, unread_count, last_updated) VALUES(?, ?, ?, ?)";
    query.prepare(sql);

    qint64 now = QDateTime::currentMSecsSinceEpoch();
    for (const auto &userInfo : userInfos) {
        query.addBindValue(userInfo.username);
        query.addBindValue(userInfo.avatarUrl);
        query.addBindValue(userInfo.unreadCount);
        query.addBindValue(now);

        if (!executeQuery(query)) {
            m_db.rollback();
            return false;
        }

        query.clear(); // 重置查询对象
        query.prepare(sql);
    }

    return m_db.commit();
}

QList<FriendData> LocalChatCache::getAllUserInfos() {
    if (!m_isInitialized) {
        qWarning() << "[LocalChatCache::getAllUserInfos] 缓存未初始化";
        return QList<FriendData>();
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT username, avatar_url, unread_count FROM users ORDER BY last_updated DESC");

    if (!executeQuery(query)) {
        return QList<FriendData>();
    }

    QList<FriendData> userInfos;
    while (query.next()) {
        FriendData userInfo;
        userInfo.username = query.value(0).toString();
        userInfo.avatarUrl = query.value(1).toString();
        userInfo.unreadCount = query.value(2).toInt();
        userInfos.append(userInfo);
    }

    return userInfos;
}

QString LocalChatCache::getUserAvatarUrl(const QString &username) {
    FriendData userInfo = getUserInfo(username);
    return userInfo.avatarUrl;
}

// ========== 群聊信息缓存方法实现 ==========
GroupInfo LocalChatCache::getGroupInfo(const QString &groupId) {
    if (!m_isInitialized) {
        qWarning() << "[LocalChatCache::getGroupInfo] 缓存未初始化";
        return GroupInfo();
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT group_name, group_avatar, members, unread_count FROM groups WHERE group_id = ?");
    query.addBindValue(groupId);

    if (!executeQuery(query)) {
        return GroupInfo();
    }

    GroupInfo groupInfo;
    groupInfo.groupId = groupId;
    if (query.next()) {
        groupInfo.groupName = query.value(0).toString();
        groupInfo.groupAvatar = query.value(1).toString();
        groupInfo.members = query.value(2).toString();
        groupInfo.unreadCount = query.value(3).toInt();
    }

    return groupInfo;
}

bool LocalChatCache::saveGroupInfo(const GroupInfo &groupInfo) {
    if (!m_isInitialized) {
        qWarning() << "[LocalChatCache::saveGroupInfo] 缓存未初始化";
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("INSERT OR REPLACE INTO groups(group_id, group_name, group_avatar, members, unread_count, last_updated) VALUES(?, ?, ?, ?, ?, ?)");
    query.addBindValue(groupInfo.groupId);
    query.addBindValue(groupInfo.groupName);
    query.addBindValue(groupInfo.groupAvatar);
    query.addBindValue(groupInfo.members);
    query.addBindValue(groupInfo.unreadCount);
    query.addBindValue(QDateTime::currentMSecsSinceEpoch());

    return executeQuery(query);
}

bool LocalChatCache::saveGroupInfos(const QList<GroupInfo> &groupInfos) {
    if (!m_isInitialized || groupInfos.isEmpty()) {
        qWarning() << "[LocalChatCache::saveGroupInfos] 缓存未初始化或群聊信息列表为空";
        return false;
    }

    if (!m_db.transaction()) {
        qWarning() << "[LocalChatCache::saveGroupInfos] 开启事务失败:" << m_db.lastError().text();
        return false;
    }

    QSqlQuery query(m_db);
    QString sql = "INSERT OR REPLACE INTO groups(group_id, group_name, group_avatar, members, unread_count, last_updated) VALUES(?, ?, ?, ?, ?, ?)";
    query.prepare(sql);

    qint64 now = QDateTime::currentMSecsSinceEpoch();
    for (const auto &groupInfo : groupInfos) {
        query.addBindValue(groupInfo.groupId);
        query.addBindValue(groupInfo.groupName);
        query.addBindValue(groupInfo.groupAvatar);
        query.addBindValue(groupInfo.members);
        query.addBindValue(groupInfo.unreadCount);
        query.addBindValue(now);

        if (!executeQuery(query)) {
            m_db.rollback();
            return false;
        }

        query.clear(); // 重置查询对象
        query.prepare(sql);
    }

    return m_db.commit();
}

QList<GroupInfo> LocalChatCache::getAllGroupInfos() {
    if (!m_isInitialized) {
        qWarning() << "[LocalChatCache::getAllGroupInfos] 缓存未初始化";
        return QList<GroupInfo>();
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT group_id, group_name, group_avatar, members, unread_count FROM groups ORDER BY last_updated DESC");

    if (!executeQuery(query)) {
        return QList<GroupInfo>();
    }

    QList<GroupInfo> groupInfos;
    while (query.next()) {
        GroupInfo groupInfo;
        groupInfo.groupId = query.value(0).toString();
        groupInfo.groupName = query.value(1).toString();
        groupInfo.groupAvatar = query.value(2).toString();
        groupInfo.members = query.value(3).toString();
        groupInfo.unreadCount = query.value(4).toInt();
        groupInfos.append(groupInfo);
    }

    return groupInfos;
}

QString LocalChatCache::getGroupAvatarUrl(const QString &groupId) {
    GroupInfo groupInfo = getGroupInfo(groupId);
    return groupInfo.groupAvatar;
}

/**
 * @brief 关闭本地缓存
 * 关闭数据库连接
 */
void LocalChatCache::close()
{
    if (m_db.isOpen()) {
        m_db.close();
        qDebug() << "本地聊天缓存已关闭";
    }
    m_isInitialized = false;
    m_username.clear();
    m_dbPath.clear();
}

/**
 * @brief 执行SQL查询
 * 封装通用的查询执行逻辑
 * @param query 查询对象
 * @param sql SQL语句（可选，如果已prepare则可不传）
 * @return 是否执行成功
 */
bool LocalChatCache::executeQuery(QSqlQuery &query, const QString &sql)
{
    if (!sql.isEmpty()) {
        if (!query.prepare(sql)) {
            qCritical() << "SQL准备失败:" << query.lastError().text() << " SQL:" << sql;
            return false;
        }
    }

    if (!query.exec()) {
        qCritical() << "SQL执行失败:" << query.lastError().text() << " SQL:" << query.lastQuery();
        return false;
    }

    return true;
}

