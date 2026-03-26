#ifndef CHATDELEGATE_H
#define CHATDELEGATE_H

#include <QStyledItemDelegate>
#include <QNetworkAccessManager>
#include <QSslConfiguration>
#include "cacheutils.h"
#include <QAbstractItemView>
#include <QPainterPath>
#include <QSize>
#include "FileAttachment.h"  // 【0113晚上新增功能：引入文件附件结构体头文件】

class ChatDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ChatDelegate(QObject *parent = nullptr);
    void setSslConfiguration(const QSslConfiguration &config);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    // 【0113晚上新增功能：声明预览图缓存路径获取方法】
    QString getCachedPreviewPath(const QString &previewUrl) const;
    // 【0113晚上新增功能：声明判断是否为文件消息的方法】
    bool isFileMessage(const QModelIndex &index) const;
    // 【0113晚上新增功能：声明预览图绘制区域获取方法】
    QRect getPreviewImageRect(const QModelIndex &index, const QStyleOptionViewItem &option) const;

private slots:
    void onAvatarDownloaded(QNetworkReply *reply);
    // 【0113晚上新增功能：声明预览图下载完成回调槽函数】
    void onPreviewDownloaded(QNetworkReply *reply) const;

private:
    QString getCachedAvatarPath(const QString &url) const;
    void drawChatBubble(QPainter *painter, const QRect &bubbleRect, bool isSelf, int triangleY) const;
    QSize calculateTextSize(const QString &text, int maxWidth) const;
    QString formatTime(const QString &rawTime) const;

    // 【0113晚上新增功能：声明预览图下载方法】
    void downloadFilePreview(const QString &previewUrl) const;
    // 【0113晚上新增功能：声明预览图尺寸计算方法】
    QSize calculatePreviewSize(const QString &previewUrl, int maxWidth) const;

    static const int AVATAR_SIZE;
    static const int ITEM_MARGIN;
    static const int BUBBLE_PADDING_H;
    static const int BUBBLE_PADDING_V;
    static const int BUBBLE_RADIUS;
    static const int TRIANGLE_SIZE;
    static const int TEXT_MAX_WIDTH;
    static const int NAME_TIME_HEIGHT;
    static const int NAME_HORIZONTAL_PAD;

    // 【0113晚上新增功能：声明预览图最大高度】
    static const int PREVIEW_MAX_HEIGHT;

    QSslConfiguration m_sslConf;
    QString m_cacheDir;

    // 【0113晚上新增功能：声明预览图缓存目录】
    QString m_previewCacheDir;

    // 【修复：仅声明静态常量，不初始化（初始化移到cpp类外）】
    static const int RequestType_Avatar;    // 头像请求标记
    static const int RequestType_Preview;   // 预览图请求标记

    // 【0115晚上新增】记录正在发起的预览图请求URL（mutable允许const函数中修改）
    mutable QSet<QString> m_pendingPreviewRequests;
    mutable QNetworkAccessManager* m_netManager;
};

#endif
