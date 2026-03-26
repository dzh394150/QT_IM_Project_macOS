#include "FileDownloader.h"
#include "CacheUtils.h"
#include <QDir>
#include <QUrl>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QNetworkReply>
#include <QFile>
#include <QFileInfo>
#include <QTimer>
#include <QRegularExpression>
#include <QDebug>
#include <QDesktopServices> // 【0117晚上修正】新增：支持打开文件夹/文件

// 【0113新增功能：实现文件下载工具类的初始化，创建网络请求管理器】
FileDownloader::FileDownloader(QObject *parent)
    : QObject(parent),
    m_netManager(new QNetworkAccessManager(this)),
    m_downloadReply(nullptr),
    m_targetFile(nullptr),
    m_totalFileSize(0),
    m_downloadedBytes(0),
    m_currentChunkStart(0),
    m_currentChunkEnd(0),
    m_isChunkDownload(false),
    m_supportRangeRequest(false),
    m_currentChunkIndex(0),
    m_totalChunks(0)
{
    // 注意：当前Qt版本不支持setMaxConnectionsPerHost方法
    // 默认每主机最多6个并发连接，已足够当前8个并发分片的需求

}

// 【0113新增功能：释放下载相关资源，避免内存泄漏和文件句柄未关闭】
FileDownloader::~FileDownloader()
{
    releaseDownloadResources();
    m_netManager->deleteLater();
}

// 【0113新增功能：开始文件下载，核心入口方法，处理目录创建、请求发起等逻辑】
void FileDownloader::startDownload(const QString &downloadUrl, const QString &userName, const QSslConfiguration &sslConf)
{
    // 【0114晚上新增】存储用户名（用于重定向后重新发起请求）
    m_currentUser = userName;
    if (downloadUrl.isEmpty()) {
        emit downloadFailed("下载URL为空，无法发起下载！");
        return;
    }

    if (userName.isEmpty()) {
        emit downloadFailed("用户名为空，无法创建下载目录！");
        return;
    }

    // 第一步：释放之前的下载资源（避免重复下载）
    releaseDownloadResources();

    // 第二步：创建下载目录
    QString downloadDir = createDownloadDir(userName);
    if (downloadDir.isEmpty()) {
        emit downloadFailed("下载目录创建失败！");
        return;
    }

    // 第三步：提取文件名并拼接保存路径
    QString fileName = extractFileName(downloadUrl);
    QDir downloadDirObj(downloadDir);
    m_savePath = downloadDirObj.filePath(fileName);

    // 第四步：创建目标文件
    m_targetFile = new QFile(m_savePath);
    if (!m_targetFile->open(QIODevice::WriteOnly)) {
        emit downloadFailed(QString("目标文件创建失败：%1").arg(m_targetFile->errorString()));
        return;
    }

    // 第五步：保存URL和SSL配置
    m_fileUrl = downloadUrl;
    m_sslConf = sslConf;

    // 【0320分片下载优化】先尝试分片下载，如果服务器不支持则降级为普通下载
    startChunkDownload(downloadUrl, userName, sslConf);
}

// 【0320分片下载新增】开始分片下载（先获取文件大小）
void FileDownloader::startChunkDownload(const QString &downloadUrl, const QString &userName, const QSslConfiguration &sslConf)
{
    Q_UNUSED(userName);
    
    // 初始化分片下载状态
    m_totalFileSize = 0;
    m_downloadedBytes = 0;
    m_currentChunkStart = 0;
    m_currentChunkEnd = 0;
    m_isChunkDownload = true;
    m_supportRangeRequest = false;

    // 发送HEAD请求获取文件大小和服务器支持情况
    QNetworkRequest req{QUrl(downloadUrl)};
    req.setSslConfiguration(sslConf);
    req.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
    
    m_downloadReply = m_netManager->head(req);

    connect(m_downloadReply, &QNetworkReply::finished, this, &FileDownloader::onFileSizeReceived);
    connect(m_downloadReply, &QNetworkReply::sslErrors, this, [=](const QList<QSslError> &errors) {
        Q_UNUSED(errors);
        m_downloadReply->ignoreSslErrors();
    });

    qDebug() << "【分片下载】发送HEAD请求，获取文件大小...";
}

// 【0320分片下载新增】处理文件大小获取响应
void FileDownloader::onFileSizeReceived()
{
    if (!m_downloadReply) return;

    bool success = (m_downloadReply->error() == QNetworkReply::NoError);

    if (success) {
        // 获取文件大小
        QVariant contentLength = m_downloadReply->header(QNetworkRequest::ContentLengthHeader);
        if (contentLength.isValid()) {
            m_totalFileSize = contentLength.toLongLong();
            qDebug() << "【分片下载】文件总大小：" << m_totalFileSize << "字节";
            
            // 预分配文件大小（减少文件系统频繁扩容）
            if (m_targetFile && m_targetFile->isOpen()) {
                m_targetFile->resize(m_totalFileSize);
            }
        }

        // 检查服务器是否支持Range请求
        QVariant acceptRanges = m_downloadReply->rawHeader("Accept-Ranges");
        if (acceptRanges.isValid() && acceptRanges.toString().toLower() == "bytes") {
            m_supportRangeRequest = true;
            qDebug() << "【分片下载】服务器支持Range请求";
        } else {
            qDebug() << "【分片下载】服务器不支持Range请求，将使用普通下载";
        }
    }

    m_downloadReply->deleteLater();
    m_downloadReply = nullptr;

    if (!success || m_totalFileSize <= 0) {
        qDebug() << "【分片下载】获取文件大小失败，降级为普通下载";
        m_isChunkDownload = false;
        // 降级为普通下载
        QNetworkRequest req{QUrl(m_fileUrl)};
        req.setSslConfiguration(m_sslConf);
        req.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
        
        m_downloadReply = m_netManager->get(req);
        connect(m_downloadReply, &QNetworkReply::finished, this, &FileDownloader::onDownloadFinished);
        connect(m_downloadReply, &QNetworkReply::readyRead, this, &FileDownloader::onDownloadReadyRead);
        connect(m_downloadReply, &QNetworkReply::downloadProgress, this, &FileDownloader::onDownloadProgress);
        connect(m_downloadReply, &QNetworkReply::sslErrors, this, [=](const QList<QSslError> &errors) {
            Q_UNUSED(errors);
            m_downloadReply->ignoreSslErrors();
        });
        return;
    }

    // 如果服务器支持Range请求且文件大于256KB，使用并发分片下载
    if (m_supportRangeRequest && m_totalFileSize > CHUNK_DOWNLOAD_SIZE) {
        qDebug() << "【分片下载】启用并发分片下载模式，分片大小：" << CHUNK_DOWNLOAD_SIZE << "字节";
        m_downloadedBytes = 0;
        m_currentChunkIndex = 0;
        m_totalChunks = (m_totalFileSize + CHUNK_DOWNLOAD_SIZE - 1) / CHUNK_DOWNLOAD_SIZE;
        qDebug() << "【并发下载】文件总大小：" << m_totalFileSize << "字节，总分片数：" << m_totalChunks;
        startConcurrentChunkDownload();
    } else {
        // 文件太小或服务器不支持，使用普通下载
        qDebug() << "【分片下载】文件太小或服务器不支持，使用普通下载";
        m_isChunkDownload = false;
        QNetworkRequest req{QUrl(m_fileUrl)};
        req.setSslConfiguration(m_sslConf);
        req.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
        
        m_downloadReply = m_netManager->get(req);
        connect(m_downloadReply, &QNetworkReply::finished, this, &FileDownloader::onDownloadFinished);
        connect(m_downloadReply, &QNetworkReply::readyRead, this, &FileDownloader::onDownloadReadyRead);
        connect(m_downloadReply, &QNetworkReply::downloadProgress, this, &FileDownloader::onDownloadProgress);
        connect(m_downloadReply, &QNetworkReply::sslErrors, this, [=](const QList<QSslError> &errors) {
            Q_UNUSED(errors);
            m_downloadReply->ignoreSslErrors();
        });
    }
}

// 【0320分片下载新增】下载下一个分片
void FileDownloader::downloadNextChunk()
{
    if (m_downloadedBytes >= m_totalFileSize) {
        // 所有分片下载完成
        qDebug() << "【分片下载】所有分片下载完成";
        if (m_targetFile && m_targetFile->isOpen()) {
            m_targetFile->close();
        }
        emit downloadFinished(m_savePath, true);
        releaseDownloadResources();
        return;
    }

    // 计算当前分片的字节范围
    m_currentChunkStart = m_downloadedBytes;
    m_currentChunkEnd = qMin(m_currentChunkStart + CHUNK_DOWNLOAD_SIZE - 1, m_totalFileSize - 1);

    // 构造Range请求头
    QString rangeHeader = QString("bytes=%1-%2").arg(m_currentChunkStart).arg(m_currentChunkEnd);
    qDebug() << "【分片下载】下载分片：Range:" << rangeHeader 
             << "（" << (m_currentChunkEnd - m_currentChunkStart + 1) << "字节）";

    QNetworkRequest req{QUrl(m_fileUrl)};
    req.setSslConfiguration(m_sslConf);
    req.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
    req.setRawHeader("Range", rangeHeader.toUtf8());

    m_downloadReply = m_netManager->get(req);

    connect(m_downloadReply, &QNetworkReply::finished, this, &FileDownloader::onChunkDownloadFinished);
    connect(m_downloadReply, &QNetworkReply::readyRead, this, &FileDownloader::onDownloadReadyRead);
    connect(m_downloadReply, &QNetworkReply::sslErrors, this, [=](const QList<QSslError> &errors) {
        Q_UNUSED(errors);
        m_downloadReply->ignoreSslErrors();
    });
}

// 【0320分片下载新增】分片下载完成回调
void FileDownloader::onChunkDownloadFinished()
{
    if (!m_downloadReply) return;

    bool success = (m_downloadReply->error() == QNetworkReply::NoError);

    if (success) {
        // 读取剩余数据并写入文件
        QByteArray remainingData = m_downloadReply->readAll();
        if (!remainingData.isEmpty() && m_targetFile && m_targetFile->isOpen()) {
            m_targetFile->write(remainingData);
            m_targetFile->flush();
        }

        // 更新已下载字节数
        m_downloadedBytes = m_currentChunkEnd + 1;

        // 发送进度信号
        emit downloadProgress(m_downloadedBytes, m_totalFileSize);

        qDebug() << "【分片下载】分片下载完成，已下载：" << m_downloadedBytes << "/" << m_totalFileSize << "字节";

        m_downloadReply->deleteLater();
        m_downloadReply = nullptr;

        // 下载下一个分片
        downloadNextChunk();
    } else {
        qCritical() << "【分片下载】分片下载失败：" << m_downloadReply->errorString();
        emit downloadFailed(m_downloadReply->errorString());
        releaseDownloadResources();
    }
}

// 【0113新增功能：创建下载目录】
QString FileDownloader::createDownloadDir(const QString &userName)
{
    QString downloadRootDir = CacheUtils::getFileDownloadRootDir(userName);
    QDir dir(downloadRootDir);
    if (!dir.mkpath(".")) {
        return QString();
    }
    return downloadRootDir;
}

// 【0113新增功能：从下载URL中提取文件名】
QString FileDownloader::extractFileName(const QString &downloadUrl) const
{
    QUrl url(downloadUrl);
    QString fileName = url.fileName();
    if (fileName.isEmpty() || !fileName.contains(".")) {
        QByteArray hash = QCryptographicHash::hash(downloadUrl.toUtf8(), QCryptographicHash::Md5);
        fileName = QString("%1.bin").arg(hash.toHex());
    }
    return fileName;
}

// 【0113新增功能：接收下载数据】
void FileDownloader::onDownloadReadyRead()
{
    if (!m_targetFile) {
        qWarning() << "onDownloadReadyRead：m_targetFile 为空";
        return;
    }
    if (!m_downloadReply) {
        qWarning() << "onDownloadReadyRead：m_downloadReply 为空";
        return;
    }
    if (!m_targetFile->isOpen()) {
        qWarning() << "onDownloadReadyRead：目标文件未打开";
        return;
    }

    QByteArray data = m_downloadReply->readAll();
    if (!data.isEmpty()) {
        qint64 writeBytes = m_targetFile->write(data);
        if (writeBytes != data.size()) {
            qWarning() << "流式写入失败：部分数据未写入";
        }
        m_targetFile->flush();
    }
}

// 【0113新增功能：转发下载进度信号】
void FileDownloader::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    // 分片下载模式下，进度由onChunkDownloadFinished处理
    if (m_isChunkDownload) {
        return;
    }
    emit downloadProgress(bytesReceived, bytesTotal);
}

// 【0113新增功能：下载完成回调】
void FileDownloader::onDownloadFinished()
{
    qDebug() << "触发FileDownloader::onDownloadFinished函数，文件下载完成";
    bool isSuccess = (m_downloadReply && m_downloadReply->error() == QNetworkReply::NoError);

    // 兜底读取剩余数据
    if (m_downloadReply && m_targetFile && m_targetFile->isOpen()) {
        QByteArray remainingData = m_downloadReply->readAll();
        if (!remainingData.isEmpty()) {
            m_targetFile->write(remainingData);
            m_targetFile->flush();
        }
    }

    // 解析真实文件名
    QString realFileName;
    if (isSuccess && m_downloadReply) {
        QVariant contentDisposition = m_downloadReply->header(QNetworkRequest::ContentDispositionHeader);
        if (contentDisposition.isValid()) {
            QByteArray dispositionBytes = contentDisposition.toByteArray();
            QString dispositionStr = QString::fromUtf8(dispositionBytes);

            QRegularExpression utf8FilenameRegex(R"(filename\*=UTF-8''([^;]+))");
            QRegularExpressionMatch utf8Match = utf8FilenameRegex.match(dispositionStr);
            if (utf8Match.hasMatch()) {
                realFileName = utf8Match.captured(1);
                realFileName = QUrl::fromPercentEncoding(realFileName.toUtf8());
            } else {
                dispositionStr = QUrl::fromPercentEncoding(dispositionStr.toUtf8());
                if (dispositionStr.contains("filename=")) {
                    realFileName = dispositionStr.split("filename=").last();
                    realFileName = realFileName.replace("\\", "").replace("\'", "").replace("\"", "").trimmed();
                }
            }

            if (!realFileName.isEmpty()) {
                realFileName = realFileName.replace(QRegularExpression("[\\\\/:*?\"<>|]"), "_");
                realFileName = realFileName.remove(QRegularExpression("[\\x00-\\x1F\\x7F-\\x9F]"));
            }
        }
    }

    // 关闭目标文件
    if (m_targetFile && m_targetFile->isOpen()) {
        m_targetFile->close();
    }

    // 文件重命名逻辑
    if (isSuccess && !realFileName.isEmpty() && !m_savePath.isEmpty()) {
        QFileInfo fileInfo(m_savePath);
        QString targetDir = fileInfo.absolutePath();
        QDir dir(targetDir);
        QString newSavePath = dir.filePath(realFileName);

        QString oldFilePath = m_savePath;

        if (!QFile::exists(oldFilePath)) {
            qWarning() << "重命名失败：旧文件不存在";
            return;
        }
        if (!dir.exists()) {
            bool dirCreateSuccess = dir.mkpath(targetDir);
            if (!dirCreateSuccess) {
                return;
            }
        }
        if (QFile::exists(newSavePath)) {
            int dotIndex = realFileName.lastIndexOf(".");
            QString baseName = realFileName.left(dotIndex);
            QString suffix = realFileName.right(realFileName.length() - dotIndex);
            realFileName = QString("%1_1%2").arg(baseName, suffix);
            newSavePath = dir.filePath(realFileName);
        }

        QTimer::singleShot(50, this, [=]() mutable {
            QFile oldFile(oldFilePath);
            if (oldFile.fileName().isEmpty() || newSavePath.isEmpty()) {
                return;
            }

            bool renameSuccess = oldFile.rename(newSavePath);
            if (renameSuccess) {
                m_savePath = newSavePath;
            } else {
                QFile newFile(newSavePath);
                if (newFile.exists()) {
                    newFile.remove();
                }
                bool copySuccess = oldFile.copy(newSavePath);
                if (copySuccess) {
                    oldFile.remove();
                    m_savePath = newSavePath;
                }
            }
        });
    }

    // 发送结果信号
    if (isSuccess) {
        if (!m_savePath.isEmpty() && QFile::exists(m_savePath)) {
            QFileInfo fileInfo(m_savePath);
            QString dirPath = fileInfo.absolutePath();
            QUrl dirUrl = QUrl::fromLocalFile(dirPath);
            QDesktopServices::openUrl(dirUrl);
        }
        emit downloadFinished(m_savePath, true);
    } else {
        QString errorMsg = m_downloadReply ? m_downloadReply->errorString() : "未知下载错误";
        emit downloadFailed(errorMsg);
    }

    // 释放资源
    releaseDownloadResources();
}

// 【0113新增功能：释放下载相关资源】
void FileDownloader::releaseDownloadResources()
{
    if (m_targetFile) {
        if (m_targetFile->isOpen()) {
            m_targetFile->close();
        }
        delete m_targetFile;
        m_targetFile = nullptr;
    }

    if (m_downloadReply) {
        if (!m_downloadReply->isFinished()) {
            m_downloadReply->abort();
        }
        m_downloadReply->deleteLater();
        m_downloadReply = nullptr;
    }

    // 【0323并发下载新增】清理并发下载相关资源
    for (auto it = m_replyToChunkMap.begin(); it != m_replyToChunkMap.end(); ++it) {
        QNetworkReply *reply = it.key();
        if (reply && !reply->isFinished()) {
            reply->abort();
        }
        reply->deleteLater();
    }
    m_replyToChunkMap.clear();
    m_inFlightChunks.clear();

    m_savePath.clear();
    m_fileUrl.clear();
    m_totalFileSize = 0;
    m_downloadedBytes = 0;
    m_currentChunkStart = 0;
    m_currentChunkEnd = 0;
    m_isChunkDownload = false;
    m_supportRangeRequest = false;
    m_currentChunkIndex = 0;
    m_totalChunks = 0;
}

// 【0114晚上新增】重定向处理
void FileDownloader::handleRedirect()
{
    if (!m_downloadReply) return;

    QVariant redirectUrlVar = m_downloadReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    QUrl redirectUrl = redirectUrlVar.toUrl();
    if (redirectUrl.isEmpty()) return;

    if (redirectUrl.isRelative()) {
        redirectUrl = m_downloadReply->request().url().resolved(redirectUrl);
    }

    qDebug() << "检测到重定向，新地址：" << redirectUrl.toString();

    releaseDownloadResources();
    startDownload(redirectUrl.toString(), m_currentUser, m_sslConf);
}

// 【0323并发下载新增】开始并发分片下载
void FileDownloader::startConcurrentChunkDownload()
{
    // 清空并发相关数据结构
    m_inFlightChunks.clear();
    m_replyToChunkMap.clear();
    
    // 开始下载第一批分片
    downloadNextChunks();
}

// 【0323并发下载新增】下载下一批分片
void FileDownloader::downloadNextChunks()
{
    // 检查是否所有分片都已下载完成
    if (m_currentChunkIndex >= m_totalChunks && m_inFlightChunks.isEmpty()) {
        // 所有分片下载完成
        qDebug() << "【并发下载】所有分片下载完成";
        
        // 关闭文件
        if (m_targetFile && m_targetFile->isOpen()) {
            m_targetFile->flush();
            m_targetFile->close();
        }
        
        // 【0325修复】并发下载完成后，尝试获取真实文件名并重命名
        // 发送一个HEAD请求获取Content-Disposition
        QNetworkRequest req{QUrl(m_fileUrl)};
        req.setSslConfiguration(m_sslConf);
        req.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
        
        QNetworkReply *headReply = m_netManager->head(req);
        connect(headReply, &QNetworkReply::finished, this, [this, headReply]() {
            QString realFileName;
                
            if (headReply->error() == QNetworkReply::NoError) {
                // 解析Content-Disposition获取真实文件名
                QVariant contentDisposition = headReply->header(QNetworkRequest::ContentDispositionHeader);
                if (contentDisposition.isValid()) {
                    QByteArray dispositionBytes = contentDisposition.toByteArray();
                    QString dispositionStr = QString::fromUtf8(dispositionBytes);
                
                    QRegularExpression utf8FilenameRegex(R"(filename\*=UTF-8''([^;]+))");
                    QRegularExpressionMatch utf8Match = utf8FilenameRegex.match(dispositionStr);
                    if (utf8Match.hasMatch()) {
                        realFileName = utf8Match.captured(1);
                        realFileName = QUrl::fromPercentEncoding(realFileName.toUtf8());
                    } else {
                        dispositionStr = QUrl::fromPercentEncoding(dispositionStr.toUtf8());
                        if (dispositionStr.contains("filename=")) {
                            realFileName = dispositionStr.split("filename=").last();
                            realFileName = realFileName.replace("\\", "").replace("\'", "").replace("\"", "").trimmed();
                        }
                    }
    
                    if (!realFileName.isEmpty()) {
                        realFileName = realFileName.replace(QRegularExpression("[\\/:*?\"<>|]"), "_");
                        realFileName = realFileName.remove(QRegularExpression("[\\x00-\\x1F\\x7F-\\x9F]"));
                    }
                }
            }
                            
            headReply->deleteLater();
                
            // 如果获取到真实文件名，重命名文件
            if (!realFileName.isEmpty() && !m_savePath.isEmpty()) {
                QFileInfo fileInfo(m_savePath);
                QString targetDir = fileInfo.absolutePath();
                QDir dir(targetDir);
                QString newSavePath = dir.filePath(realFileName);
    
                QString oldFilePath = m_savePath;
    
                if (QFile::exists(oldFilePath)) {
                    if (QFile::exists(newSavePath)) {
                        int dotIndex = realFileName.lastIndexOf(".");
                        QString baseName = realFileName.left(dotIndex);
                        QString suffix = realFileName.right(realFileName.length() - dotIndex);
                        realFileName = QString("%1_1%2").arg(baseName, suffix);
                        newSavePath = dir.filePath(realFileName);
                    }
    
                    QFile oldFile(oldFilePath);
                    if (oldFile.rename(newSavePath)) {
                        m_savePath = newSavePath;
                        qDebug() << "【并发下载】文件重命名成功：" << newSavePath;
                    } else {
                        // 重命名失败，尝试复制后删除
                        QFile newFile(newSavePath);
                        if (newFile.exists()) {
                            newFile.remove();
                        }
                        if (oldFile.copy(newSavePath)) {
                            oldFile.remove();
                            m_savePath = newSavePath;
                            qDebug() << "【并发下载】文件复制重命名成功：" << newSavePath;
                        } else {
                            qWarning() << "【并发下载】文件重命名失败";
                        }
                    }
                }
            }
                
            // 发送进度和完成信号
            emit downloadProgress(m_totalFileSize, m_totalFileSize);
                
            // 打开存放目录
            if (!m_savePath.isEmpty() && QFile::exists(m_savePath)) {
                QFileInfo fileInfo(m_savePath);
                QString dirPath = fileInfo.absolutePath();
                QUrl dirUrl = QUrl::fromLocalFile(dirPath);
                QDesktopServices::openUrl(dirUrl);
            }
                
            emit downloadFinished(m_savePath, true);
            releaseDownloadResources();
        });
        connect(headReply, &QNetworkReply::sslErrors, this, [headReply](const QList<QSslError> &errors) {
            Q_UNUSED(errors);
            headReply->ignoreSslErrors();
        });
        return;
    }
    
    // 并发下载下一批分片
    while (m_inFlightChunks.size() < m_maxConcurrentChunks && m_currentChunkIndex < m_totalChunks) {
        int chunkIndex = m_currentChunkIndex;
        
        // 计算当前分片的字节范围
        qint64 chunkStart = chunkIndex * CHUNK_DOWNLOAD_SIZE;
        qint64 chunkEnd = qMin(chunkStart + CHUNK_DOWNLOAD_SIZE - 1, m_totalFileSize - 1);
        
        // 构造Range请求头
        QString rangeHeader = QString("bytes=%1-%2").arg(chunkStart).arg(chunkEnd);
        qDebug() << "【并发下载】下载分片" << chunkIndex << ": Range:" << rangeHeader 
                 << "（" << (chunkEnd - chunkStart + 1) << "字节）";
        
        // 创建网络请求
        QNetworkRequest req{QUrl(m_fileUrl)};
        req.setSslConfiguration(m_sslConf);
        req.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
        req.setRawHeader("Range", rangeHeader.toUtf8());
        
        // 发送请求
        QNetworkReply *reply = m_netManager->get(req);
        
        // 记录分片信息
        m_inFlightChunks.insert(chunkIndex);
        m_replyToChunkMap[reply] = chunkIndex;
        
        // 连接信号
        connect(reply, &QNetworkReply::finished, this, &FileDownloader::onConcurrentChunkDownloadFinished);
        connect(reply, &QNetworkReply::sslErrors, this, [=](const QList<QSslError> &errors) {
            Q_UNUSED(errors);
            reply->ignoreSslErrors();
        });
        
        // 移动到下一个分片
        m_currentChunkIndex++;
    }
    
    qDebug() << "【并发下载】当前并发数：" << m_inFlightChunks.size() << "/" << m_maxConcurrentChunks;
}

// 【0323并发下载新增】并发分片下载完成回调
void FileDownloader::onConcurrentChunkDownloadFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    
    handleConcurrentChunkFinished(reply);
}

// 【0323并发下载新增】处理并发分片下载完成
void FileDownloader::handleConcurrentChunkFinished(QNetworkReply *reply)
{
    if (!reply) return;
    
    // 获取分片索引
    if (!m_replyToChunkMap.contains(reply)) {
        reply->deleteLater();
        return;
    }
    
    int chunkIndex = m_replyToChunkMap[reply];
    bool success = (reply->error() == QNetworkReply::NoError);
    
    if (success) {
        // 读取分片数据
        QByteArray chunkData = reply->readAll();
        
        // 【0323优化】直接写入文件，避免内存缓存过大
        if (m_targetFile && m_targetFile->isOpen()) {
            // 定位到正确的位置写入
            qint64 writePos = chunkIndex * CHUNK_DOWNLOAD_SIZE;
            if (m_targetFile->seek(writePos)) {
                m_targetFile->write(chunkData);
                // 减少flush频率，提高写入性能
            }
        }
        
        // 更新已下载字节数
        m_downloadedBytes += chunkData.size();
        
        // 发送进度信号
        emit downloadProgress(m_downloadedBytes, m_totalFileSize);
        
        qDebug() << "【并发下载】分片" << chunkIndex << "下载完成，已下载：" << m_downloadedBytes << "/" << m_totalFileSize << "字节";
    } else {
        qCritical() << "【并发下载】分片" << chunkIndex << "下载失败：" << reply->errorString();
        emit downloadFailed(reply->errorString());
        releaseDownloadResources();
        return;
    }
    
    // 清理资源
    m_inFlightChunks.remove(chunkIndex);
    m_replyToChunkMap.remove(reply);
    reply->deleteLater();
    
    // 下载下一批分片
    downloadNextChunks();
}
