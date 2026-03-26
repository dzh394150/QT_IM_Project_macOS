#ifndef FILECHUNKINFO_H
#define FILECHUNKINFO_H

#include <QString>

// 分片信息结构体（封装单个分片核心数据）
struct FileChunkInfo {
    QString fileUniqueId;       // 文件全局唯一ID（整个文件唯一）
    int chunkIndex;             // 当前分片编号（从0开始）
    int totalChunks;            // 总分片数
    bool isLastChunk;           // 是否为最后一个分片
    QString chunkBase64;        // 当前分片Base64编码
    QString originalFileName;   // 原始文件名
    QString fileSuffix;         // 文件后缀
    QString msgtype;            // 文件类型
    qint64 fileTotalSize;       // 文件总大小（字节）
    QString globalMsgId;        // 分片全局消息ID
    QString fromUser;           // 发送方用户名
    QString toUser;             // 接收方用户名
};

#endif // FILECHUNKINFO_H
