#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <QString>
#include <QStringList>

// 【0113新增功能：通用文件工具类，封装文件名提取、后缀判断、路径拼接等复用逻辑】
class FileUtils
{
public:
    // 从文件路径/URL中提取文件后缀（小写，不带小数点）
    static QString extractFileSuffix(const QString &pathOrUrl);

    // 判断文件是否为图片格式（支持常见图片后缀）
    static bool isImageFile(const QString &pathOrUrl);

    // 拼接路径（自动处理不同系统的路径分隔符）
    static QString joinPath(const QString &path1, const QString &path2);

    // 从路径/URL中提取文件名（包含后缀）
    static QString extractFileName(const QString &pathOrUrl);

private:
    // 私有构造函数，禁止实例化（纯静态工具类）
    FileUtils() = delete;
    ~FileUtils() = delete;

    // 常见图片后缀列表
    static const QStringList IMAGE_SUFFIXES;
};

#endif // FILEUTILS_H
