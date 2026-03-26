#include "CacheUtils.h"
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

// 辅助方法：创建目录（递归创建，返回是否创建成功/目录已存在）
bool CacheUtils::createDirIfNotExists(const QString &dirPath)
{
    QDir dir(dirPath);
    if (dir.exists()) {
        return true; // 目录已存在，无需创建
    }

    // 递归创建目录（父目录不存在也会自动创建）
    bool isCreated = dir.mkpath(dirPath);
    if (!isCreated) {
        qDebug() << "【缓存目录创建失败】路径：" << dirPath << "（无写入权限或路径非法）";
    } else {
        qDebug() << "【缓存目录创建成功】路径：" << dirPath;
    }
    return isCreated;
}

// 获取头像缓存目录（自动创建）
QString CacheUtils::getAvatarCacheDir()
{
    // 选择系统可写目录（避免权限问题），拼接自定义缓存路径
    QString baseDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString avatarDirPath = baseDir + "/ChatCache/Avatar";

    // 自动创建目录
    createDirIfNotExists(avatarDirPath);

    return avatarDirPath;
}

// 获取文件预览图缓存目录（自动创建）
QString CacheUtils::getFilePreviewCacheDir()
{
    QString baseDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString previewDirPath = baseDir + "/ChatCache/Preview";

    // 自动创建目录
    createDirIfNotExists(previewDirPath);

    return previewDirPath;
}

// 【补充：获取文件下载根目录（跨平台适配）】
QString CacheUtils::getFileDownloadRootDir(const QString &userName)
{
    if (userName.isEmpty()) {
        qDebug() << "【文件下载根目录】用户名为空，无法构建路径";
        return QString();
    }

    QString downloadRootDir;

    // 跨平台适配
    #ifdef Q_OS_WIN
        // Windows端：运行目录/download_file_用户名/
        QString appRunDir = QDir::currentPath();
        downloadRootDir = appRunDir + QString("/download_file_%1/").arg(userName);
    #elif defined(Q_OS_MACOS)
        // macOS端：下载目录/ChatDownload_用户名/
        downloadRootDir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + QString("/ChatDownload_%1/").arg(userName);
    #else
        // 其他平台：使用标准下载目录
        downloadRootDir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + QString("/ChatDownload_%1/").arg(userName);
    #endif

    // 注：此处仅返回路径，不创建目录（创建目录的职责交给 FileDownloader::createDownloadDir）
    qDebug() << "【文件下载根目录】构建完成：" << downloadRootDir;
    return downloadRootDir;
}
