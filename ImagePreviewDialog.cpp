#include "ImagePreviewDialog.h"
#include "ui_ImagePreviewDialog.h"  // 引入UI生成文件（用户需自行通过.ui文件生成）
#include "CacheUtils.h"
#include <QFile>
#include <QCryptographicHash>
#include <QNetworkRequest>
#include <QPixmap>
#include <QMessageBox>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>

// 构造函数：初始化UI和网络管理器
ImagePreviewDialog::ImagePreviewDialog(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::ImagePreviewDialog),  // 初始化UI指针（关联.ui文件控件）
    m_netManager(new QNetworkAccessManager(this)),
    m_isDragging(false)
{
    // 加载UI文件（用户自行绘制的UI布局将被初始化）
    ui->setupUi(this);

    // 正确写法：保留QDialog默认窗口标志，仅添加无边框属性
    this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(ui->shadowwidget);
    shadow->setColor(QColor(0, 0, 0, 80));
    shadow->setBlurRadius(15);
    shadow->setOffset(0, 0);
    ui->shadowwidget->setGraphicsEffect(shadow);

    // 允许鼠标跟踪
    setMouseTracking(true);

    // 绑定核心信号槽
    // 1. 网络请求完成信号：绑定大图下载完成回调
    // connect(m_netManager, &QNetworkAccessManager::finished,
    //        this, &ImagePreviewDialog::onImageDownloadFinished);
    // 2. 关闭按钮点击信号：绑定ui->btnclose，实现窗口关闭
    connect(ui->btnclose, &QPushButton::clicked,
            this, &ImagePreviewDialog::onBtnCloseClicked);
}

// 析构函数：释放UI指针和网络资源，避免内存泄漏
ImagePreviewDialog::~ImagePreviewDialog()
{
    delete ui;  // 释放UI资源（必须优先释放）
    m_netManager->deleteLater();  // 安全释放网络管理器
}

void ImagePreviewDialog::setImageUrl(const QString &imageUrl, const QSslConfiguration &sslConf)
{
    // 合法性校验：URL为空则提示并返回
    if (imageUrl.isEmpty()) {
        QMessageBox::warning(this, "警告", "大图URL为空，无法进行预览！");
        ui->imageLabel->setText("大图URL为空，无法预览");
        return;
    }

    // 保存目标URL和SSL配置
    m_targetImageUrl = imageUrl;
    m_sslConf = sslConf;

    // 第一步：检查本地缓存是否存在
    QString cachePath = getCachedImagePath(imageUrl);
    if (QFile::exists(cachePath)) {
        loadCachedImage(cachePath);  // 缓存存在，直接加载缓存
        return;
    }

    // 第二步：缓存不存在，发起HTTPS下载请求
    QNetworkRequest req{QUrl(imageUrl)};
    req.setSslConfiguration(m_sslConf);  // 配置SSL，支持HTTPS

    // 1. 接收单个请求的reply对象（关键：与成功代码保持一致）
    QNetworkReply *reply = m_netManager->get(req);

    // 2. 绑定单个reply的sslErrors信号，处理SSL握手错误（核心：解决证书链问题）
    connect(reply, &QNetworkReply::sslErrors, this, [=](const QList<QSslError> &errors) {
        qDebug() << "大图请求SSL错误详情：";
        for (const QSslError &error : errors) {
            qDebug() << error.errorString();
        }
        // 测试场景：忽略SSL错误，强制继续请求（让握手失败的请求恢复执行）
        reply->ignoreSslErrors();
    });

    // 3. 绑定单个reply的finished信号，处理请求结果（替换全局m_netManager的finished）
    connect(reply, &QNetworkReply::finished, this, [=]() {
        // 直接调用原槽函数的逻辑，或把原槽函数内容迁移到这里
        this->onImageDownloadFinished(reply);
    });

    // 显示加载提示
    ui->imageLabel->setAlignment(Qt::AlignCenter);
    ui->imageLabel->setText("正在加载大图，请稍候...");
}

// 辅助方法：获取大图的本地缓存路径（复用MD5哈希逻辑，与头像/预览图缓存格式统一）
QString ImagePreviewDialog::getCachedImagePath(const QString &imageUrl) const
{
    // MD5哈希URL，生成唯一缓存文件名
    QByteArray hash = QCryptographicHash::hash(imageUrl.toUtf8(), QCryptographicHash::Md5);
    // 拼接缓存目录（依赖CacheUtils的文件缓存目录）
    return QString("%1/%2.png").arg(CacheUtils::getFilePreviewCacheDir(), hash.toHex());
}

// 辅助方法：加载本地缓存中的大图并显示到ui->imageLabel
void ImagePreviewDialog::loadCachedImage(const QString &cachePath)
{
    QPixmap pixmap;
    // 加载缓存文件
    if (pixmap.load(cachePath)) {
        drawPreviewImage(pixmap);  // 加载成功，绘制到控件
    } else {
        // 缓存损坏，显示错误提示
        ui->imageLabel->setAlignment(Qt::AlignCenter);
        ui->imageLabel->setText("本地缓存图片损坏，无法预览！");
    }
}

// 辅助方法：绘制大图到ui->imageLabel（保持宽高比，适配控件尺寸，避免变形）
void ImagePreviewDialog::drawPreviewImage(const QPixmap &pixmap)
{
    // 合法性校验：空图片直接返回
    if (pixmap.isNull()) {
        ui->imageLabel->setAlignment(Qt::AlignCenter);
        ui->imageLabel->setText("大图加载失败，无法预览！");
        return;
    }

    // 保存大图到成员变量（用于后续重绘或其他操作）
    m_previewPixmap = pixmap;

    // 缩放图片：适配ui->imageLabel尺寸，保持宽高比，平滑缩放
    QPixmap scaledPixmap = pixmap.scaled(
        ui->imageLabel->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation  // 平滑缩放，提升显示效果
        );

    // 显示缩放后的图片到ui->imageLabel
    ui->imageLabel->setPixmap(scaledPixmap);
    ui->imageLabel->setAlignment(Qt::AlignCenter);
}

// 槽函数：大图下载完成回调，处理下载结果
void ImagePreviewDialog::onImageDownloadFinished(QNetworkReply *reply)
{
    qDebug() << "大图下载完成后回调函数触发";
    // 第一步：检查下载错误
    if (reply->error() != QNetworkReply::NoError) {
        QString errorMsg = QString("大图下载失败：%1").arg(reply->errorString());
        qDebug() << "大图下载失败错误提示：" << errorMsg;
        ui->imageLabel->setAlignment(Qt::AlignCenter);
        ui->imageLabel->setText(errorMsg);

        reply->deleteLater();  // 释放reply资源
        return;
    }

    // 第二步：读取下载数据并转换为图片
    QByteArray imageData = reply->readAll();
    QPixmap pixmap;
    if (!pixmap.loadFromData(imageData)) {
        ui->imageLabel->setAlignment(Qt::AlignCenter);
        ui->imageLabel->setText("大图数据损坏，无法预览！");
        reply->deleteLater();  // 释放reply资源
        return;
    }

    // 第三步：保存下载的图片到本地缓存（便于后续快速加载）
    QString cachePath = getCachedImagePath(m_targetImageUrl);
    pixmap.save(cachePath, "PNG");  // 以PNG格式保存缓存

    // 第四步：绘制并显示大图到ui->imageLabel
    drawPreviewImage(pixmap);

    // 第五步：释放reply资源，避免内存泄漏
    reply->deleteLater();
}

// 槽函数：响应ui->btnclose点击，关闭预览窗口
void ImagePreviewDialog::onBtnCloseClicked()
{
    // 清空大图缓存和控件显示
    m_previewPixmap = QPixmap();
    ui->imageLabel->clear();

    // 关闭当前弹窗
    this->close();
}

// ================= 拖动实现 =================
void ImagePreviewDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        int dragAreaHeight = 35;
        int closeBtnWidth = 50;

        // 判断是否在可拖动区域（上方35px，排除最右50px）
        if (event->pos().y() < dragAreaHeight &&
            event->pos().x() < width() - closeBtnWidth)
        {
            m_isDragging = true;
            m_dragStartPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
            event->accept(); // 消费事件，不传递给 MainWindow
            return;
        }
    }

    QWidget::mousePressEvent(event);
}

void ImagePreviewDialog::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPosition().toPoint() - m_dragStartPosition);
        event->accept(); // 消费事件
        return;
    }

    QWidget::mouseMoveEvent(event);
}

void ImagePreviewDialog::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        event->accept();
    }

    QWidget::mouseReleaseEvent(event);
}
