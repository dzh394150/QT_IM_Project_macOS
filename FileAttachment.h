#ifndef FILEATTACHMENT_H
#define FILEATTACHMENT_H

#include <QString>
// 关键补充：包含Qt元对象头文件，识别Q_DECLARE_METATYPE宏
#include <QMetaType>
// 【0113新增功能：封装文件消息的核心URL和元信息，避免重复解析JSON，提升代码复用性】
struct FileAttachment
{
    // 核心URL（从服务端JSON中解析得到）
    QString downloadUrl;  // 文件下载URL（对应download_url）
    QString originUrl;    // 文件原图/原文件URL（对应origin_url）
    QString previewUrl;   // 文件预览图URL（对应preview_url）

    // 文件元信息（辅助渲染和交互，解析URL或后端数据得到）
    QString fileName;     // 文件名（从URL中提取）
    QString fileSuffix;   // 文件后缀（如png、zip、mp4）
    bool isImageFile;     // 是否为图片文件（用于区分大图查看/文件下载逻辑）

    // 构造函数（初始化默认值，避免空指针问题）
    FileAttachment() : isImageFile(false) {}
};

// 关键：添加该声明，允许FileAttachment转为QVariant存入模型
Q_DECLARE_METATYPE(FileAttachment)

#endif // FILEATTACHMENT_H
