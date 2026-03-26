#ifndef CACHEUTILS_H
#define CACHEUTILS_H

#include <QString>

// 缓存工具类：负责提供头像/预览图缓存目录、文件下载根目录，自动创建目录
class CacheUtils
{
public:
    // 获取头像缓存目录（自动创建）
    static QString getAvatarCacheDir();

    // 获取文件预览图缓存目录（自动创建）
    static QString getFilePreviewCacheDir();

    // 【补充：获取文件下载根目录（对应你的需求：运行目录/download_file_用户名/）】
    static QString getFileDownloadRootDir(const QString &userName);

private:
    // 辅助方法：创建目录（递归创建，父目录不存在也自动创建）
    static bool createDirIfNotExists(const QString &dirPath);
};

#endif // CACHEUTILS_H
