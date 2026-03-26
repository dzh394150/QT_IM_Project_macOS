#ifndef FILETRANSFERWORKER_H
#define FILETRANSFERWORKER_H

#include <QObject>
#include <QFile>
#include <QByteArray>
#include "FileChunkInfo.h"

// 分片大小常量（与mainwindow.cpp保持一致）
const qint64 FILE_CHUNK_SIZE = 256 * 1024; // 256KB的分片大小

class FileTransferWorker : public QObject
{
    Q_OBJECT

public:
    explicit FileTransferWorker(QObject *parent = nullptr);
    ~FileTransferWorker();

public slots:
    // 处理文件分片读取和WebSocket帧封装
    void processFileChunk(QFile *file, qint64 fileTotalSize, qint64 hasTransferred, 
                         int chunkIndex, int totalChunks, const QString &fileUniqueId,
                         const QString &fileName, const QString &fileType, 
                         const QString &fromUser, const QString &toUser);

signals:
    // 分片处理完成信号
    void chunkProcessed(const FileChunkInfo &chunkInfo, const QByteArray &webSocketFrame);
    // 处理出错信号
    void processError(const QString &errorMessage);

private:
    // WebSocket帧封装方法
    QByteArray createWebSocketFrame(const QByteArray &payload);
};

#endif // FILETRANSFERWORKER_H
