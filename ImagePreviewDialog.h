#ifndef IMAGEPREVIEWDIALOG_H
#define IMAGEPREVIEWDIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>
#include <QSslConfiguration>

// 前置声明UI类（由Qt Designer生成，用户需自行通过.ui文件生成对应ui_ImagePreviewDialog.h）
namespace Ui {
class ImagePreviewDialog;
}

// 大图预览弹窗：仅操控UI文件中的ui->imageLabel和ui->btnclose控件
class ImagePreviewDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ImagePreviewDialog(QWidget *parent = nullptr);
    ~ImagePreviewDialog() override;

    // 核心接口：设置要预览的大图URL和SSL配置（用于HTTPS请求）
    void setImageUrl(const QString &imageUrl, const QSslConfiguration &sslConf);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    // 大图下载完成回调：处理下载结果、保存缓存、显示图片
    void onImageDownloadFinished(QNetworkReply *reply);
    // 关闭窗口槽函数：绑定ui->btnclose的点击信号
    void onBtnCloseClicked();

private:
    // 辅助方法：获取大图的本地缓存路径（MD5哈希命名，与其他缓存逻辑一致）
    QString getCachedImagePath(const QString &imageUrl) const;
    // 辅助方法：加载本地缓存中的大图并显示到ui->imageLabel
    void loadCachedImage(const QString &cachePath);
    // 辅助方法：绘制大图到ui->imageLabel（保持宽高比，避免变形）
    void drawPreviewImage(const QPixmap &pixmap);

    // 核心成员变量
    Ui::ImagePreviewDialog *ui;                  // UI指针（操控.ui文件生成的控件）
    QNetworkAccessManager *m_netManager;         // 网络请求管理器（用于下载大图）
    QSslConfiguration m_sslConf;                 // SSL配置（兼容后端HTTPS请求）
    QString m_targetImageUrl;                    // 目标大图的URL
    QPixmap m_previewPixmap;                     // 存储加载完成的大图（用于缓存和重绘）

    // 补充拖动相关成员变量的声明
    bool m_isDragging;
    QPoint m_dragStartPosition;
};

#endif // IMAGEPREVIEWDIALOG_H
