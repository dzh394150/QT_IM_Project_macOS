#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H

#include <QString>
#include <QDateTime>
#include "FileAttachment.h"  // 【0113晚上新增功能：引入文件附件结构体头文件，支撑文件消息数据存储】

struct ChatMessage {
    enum MessageType {
        Text,
        Image,
        File,
        Video
    };

    enum MessageOwner {
        Self,
        Other,
        System
    };

    MessageType type;
    MessageOwner owner;
    QString content;       // 文本内容或文件路径
    QString senderName;    // 发送者昵称
    QString receiverName;  // 接收者昵称（用于本地缓存）
    QString avatarPath;    // 头像路径或URL
    QDateTime timestamp;   // 时间戳
    bool isRead;           // 是否已读（用于本地缓存）
    bool isSynced;         // 是否已同步到云端（用于本地缓存）
    FileAttachment fileAttach;  // 【0113晚上新增功能：添加文件附件字段，存储文件URL和元信息】

    // 构造函数，初始化默认值
    ChatMessage() : type(Text), owner(Other), isRead(false), isSynced(false) {}
};

#endif // CHATMESSAGE_H
