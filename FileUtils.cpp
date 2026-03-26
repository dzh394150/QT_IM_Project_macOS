#include "FileUtils.h"
#include <QUrl>
#include <QFileInfo>
#include <QDir>
// 【0113新增功能：定义常见图片后缀列表，用于图片文件判断】
const QStringList FileUtils::IMAGE_SUFFIXES = {
    "png", "jpg", "jpeg", "gif", "bmp", "webp", "tiff"
};

// 【0113新增功能：提取文件后缀，统一转为小写，去除小数点，兼容路径和URL】
QString FileUtils::extractFileSuffix(const QString &pathOrUrl)
{
    if (pathOrUrl.isEmpty()) {
        return QString();
    }

    // 先判断是否为URL
    QUrl url(pathOrUrl);
    if (url.isValid() && !url.fileName().isEmpty()) {
        return QFileInfo(url.fileName()).suffix().toLower();
    }

    // 再处理本地路径
    return QFileInfo(pathOrUrl).suffix().toLower();
}

// 【0113新增功能：判断文件是否为图片，基于后缀匹配，兼容路径和URL】
bool FileUtils::isImageFile(const QString &pathOrUrl)
{
    QString suffix = extractFileSuffix(pathOrUrl);
    return IMAGE_SUFFIXES.contains(suffix);
}

// 【0113新增功能：拼接两个路径，自动处理路径分隔符，兼容多系统】
QString FileUtils::joinPath(const QString &path1, const QString &path2)
{
    if (path1.isEmpty()) {
        return path2;
    }
    if (path2.isEmpty()) {
        return path1;
    }

    // 自动拼接路径，QDir::cleanPath会处理多余的分隔符
    return QDir::cleanPath(QString("%1/%2").arg(path1, path2));
}

// 【0113新增功能：提取文件名（包含后缀），兼容本地路径和网络URL】
QString FileUtils::extractFileName(const QString &pathOrUrl)
{
    if (pathOrUrl.isEmpty()) {
        return QString();
    }

    // 先判断是否为URL
    QUrl url(pathOrUrl);
    if (url.isValid() && !url.fileName().isEmpty()) {
        return url.fileName();
    }

    // 再处理本地路径
    return QFileInfo(pathOrUrl).fileName();
}
