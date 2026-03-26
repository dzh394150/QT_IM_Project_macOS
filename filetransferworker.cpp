#include "filetransferworker.h"

FileTransferWorker::FileTransferWorker(QObject *parent) : QObject(parent)
{
}

FileTransferWorker::~FileTransferWorker()
{
}

void FileTransferWorker::processFileChunk(QFile *file, qint64 fileTotalSize, qint64 hasTransferred, 
                                         int chunkIndex, int totalChunks, const QString &fileUniqueId,
                                         const QString &fileName, const QString &fileType, 
                                         const QString &fromUser, const QString &toUser)
{
    // 实现文件分片处理逻辑
    QByteArray chunkData;
    qint64 chunkSize = FILE_CHUNK_SIZE;
    qint64 remainingBytes = fileTotalSize - hasTransferred;
    if (remainingBytes < chunkSize) {
        chunkSize = remainingBytes;
    }
    
    if (file && file->isOpen()) {
        file->seek(hasTransferred);
        chunkData = file->read(chunkSize);
    } else {
        emit processError("无法打开文件");
        return;
    }

    // 填充FileChunkInfo结构体
    FileChunkInfo chunkInfo;
    chunkInfo.fileUniqueId = fileUniqueId;
    chunkInfo.chunkIndex = chunkIndex;
    chunkInfo.totalChunks = totalChunks;
    chunkInfo.isLastChunk = (chunkIndex == totalChunks - 1);
    chunkInfo.chunkBase64 = chunkData.toBase64();
    chunkInfo.originalFileName = fileName;
    chunkInfo.fileSuffix = fileName.right(fileName.length() - fileName.lastIndexOf(".") - 1);
    chunkInfo.msgtype = fileType;
    chunkInfo.fileTotalSize = fileTotalSize;
    chunkInfo.fromUser = fromUser;
    chunkInfo.toUser = toUser;

    // 发送信号，不封装WebSocket帧，由主线程处理
    emit chunkProcessed(chunkInfo, QByteArray());
}

QByteArray FileTransferWorker::createWebSocketFrame(const QByteArray &payload)
{
    // 实现WebSocket帧封装逻辑
    // 这里只是一个示例实现，实际逻辑需要根据WebSocket协议编写
    QByteArray frame;
    // 帧头
    frame.append(0x82); // 文本帧
    
    // 长度
    if (payload.size() < 126) {
        frame.append(payload.size());
    } else if (payload.size() < 65536) {
        frame.append(126);
        frame.append((payload.size() >> 8) & 0xFF);
        frame.append(payload.size() & 0xFF);
    } else {
        frame.append(127);
        for (int i = 7; i >= 0; --i) {
            frame.append((payload.size() >> (i * 8)) & 0xFF);
        }
    }
    
    // 负载
    frame.append(payload);
    
    return frame;
}
