#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslConfiguration>
#include <QFile>
#include <QString>
#include <QSet>
#include <QMap>

// 分片下载大小常量（2MB）- 优化：增加分片大小，减少请求次数
const qint64 CHUNK_DOWNLOAD_SIZE = 2 * 1024 * 1024;

// 【0113新增功能：声明文件下载工具类，封装目录创建、流式下载、进度监听等核心功能】
class FileDownloader : public QObject
{
    Q_OBJECT
public:
    explicit FileDownloader(QObject *parent = nullptr);
    ~FileDownloader() override;

    // 开始下载文件（参数：下载URL、用户名、SSL配置）
    void startDownload(const QString &downloadUrl, const QString &userName, const QSslConfiguration &sslConf);

 signals:
    // 下载进度信号（已下载字节数、总字节数）
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    // 下载完成信号（保存路径、是否成功）
    void downloadFinished(const QString &savePath, bool isSuccess);
    // 下载失败信号（错误信息）
    void downloadFailed(const QString &errorMsg);

private slots:
    // 接收下载数据（流式写入本地文件）
    void onDownloadReadyRead();
    // 下载进度更新
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    // 下载完成回调
    void onDownloadFinished();

    void handleRedirect(); // 【0114晚上新增】重定向处理槽函数

    // 【0320分片下载新增】分片下载完成回调
    void onChunkDownloadFinished();
    // 【0323并发下载新增】并发分片下载完成回调
    void onConcurrentChunkDownloadFinished();

private:
    // 【0113新增功能：创建下载目录（递归创建，支持多级目录）】
    QString createDownloadDir(const QString &userName);
    // 【0113新增功能：从下载URL中提取文件名，用于本地保存】
    QString extractFileName(const QString &downloadUrl) const;
    // 关闭并释放下载相关资源
    void releaseDownloadResources();

    // 【0320分片下载新增】开始分片下载（先获取文件大小）
    void startChunkDownload(const QString &downloadUrl, const QString &userName, const QSslConfiguration &sslConf);
    // 【0320分片下载新增】下载下一个分片
    void downloadNextChunk();
    // 【0320分片下载新增】处理文件大小获取响应
    void onFileSizeReceived();
    // 【0323并发下载新增】开始并发分片下载
    void startConcurrentChunkDownload();
    // 【0323并发下载新增】下载下一批分片
    void downloadNextChunks();
    // 【0323并发下载新增】处理并发分片下载完成
    void handleConcurrentChunkFinished(QNetworkReply *reply);

    // 核心成员变量
    QNetworkAccessManager *m_netManager;  // 网络请求管理器
    QSslConfiguration m_sslConf;          // SSL配置（兼容HTTPS）
    QNetworkReply *m_downloadReply;       // 当前下载响应
    QFile *m_targetFile;                  // 目标本地文件
    QString m_savePath;                   // 文件保存路径
    QString m_currentUser;                // 【0114晚上新增】存储用户名，用于重定向后重新发起请求

    // 【0320分片下载新增】分片下载相关成员变量
    QString m_fileUrl;                    // 文件下载URL
    qint64 m_totalFileSize;               // 文件总大小
    qint64 m_downloadedBytes;             // 已下载的总字节数
    qint64 m_currentChunkStart;           // 当前分片的起始字节位置
    qint64 m_currentChunkEnd;             // 当前分片的结束字节位置
    bool m_isChunkDownload;               // 是否为分片下载模式
    bool m_supportRangeRequest;           // 服务器是否支持Range请求
    
    // 【0323并发下载新增】并发下载相关成员变量
    const int m_maxConcurrentChunks = 8;  // 最大并发分片数 - 优化：增加并发数，充分利用带宽
    int m_currentChunkIndex;              // 当前要下载的分片索引
    int m_totalChunks;                    // 总分片数
    QSet<int> m_inFlightChunks;           // 正在下载的分片索引集合
    QMap<QNetworkReply*, int> m_replyToChunkMap; // 网络响应到分片索引的映射
};

#endif // FILEDOWNLOADER_H
