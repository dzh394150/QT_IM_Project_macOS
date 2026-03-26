#include "FriendRequestDelegate.h"
// 新增：包含绘制文本所需的头文件
#include <QPainterPath>
#include <QFontMetrics>

FriendRequestDelegate::FriendRequestDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
    connect(&m_netManager, &QNetworkAccessManager::finished,
            this, &FriendRequestDelegate::onAvatarDownloaded);
    
    // 初始化本地头像缓存目录
    m_avatarCacheDir = CacheUtils::getAvatarCacheDir();
    qDebug() << "好友申请列表头像缓存目录:" << m_avatarCacheDir;
}

// 生成头像本地缓存文件路径
QString FriendRequestDelegate::getAvatarCachePath(const QString &url) const
{
    QByteArray hash = QCryptographicHash::hash(url.toUtf8(), QCryptographicHash::Md5);
    return m_avatarCacheDir + "/" + hash.toHex() + ".jpg";
}

// 从本地缓存加载头像
QPixmap FriendRequestDelegate::loadAvatarFromLocalCache(const QString &url) const
{
    QString cachePath = getAvatarCachePath(url);
    QPixmap avatar;
    if (avatar.load(cachePath)) {
        qDebug() << "✅ 好友申请从本地缓存加载头像成功:" << cachePath;
        return avatar;
    }
    qDebug() << "❌ 好友申请从本地缓存加载头像失败:" << cachePath;
    return QPixmap();
}

// 保存头像到本地缓存
void FriendRequestDelegate::saveAvatarToLocalCache(const QString &url, const QPixmap &avatar) const
{
    QString cachePath = getAvatarCachePath(url);
    if (avatar.save(cachePath, "JPG", 80)) {
        qDebug() << "💾 好友申请头像已保存到本地缓存:" << cachePath;
    } else {
        qDebug() << "❌ 好友申请头像保存到本地缓存失败:" << cachePath;
    }
}

void FriendRequestDelegate::paint(QPainter *painter,
                                  const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const
{
    if (!index.isValid()) return;

    painter->save();

    // 背景
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, QColor(220, 220, 220)); // RGB 浅灰色
    } else {
        painter->fillRect(option.rect, option.palette.base());
    }

    // 数据
    QString fromUser = index.data(Qt::UserRole + 1).toString();
    QString avatarUrl = index.data(Qt::UserRole + 2).toString();
    QString message = index.data(Qt::UserRole + 3).toString();
    int status = index.data(Qt::UserRole + 4).toInt(); // 获取状态（StatusRole=Qt::UserRole+4）

    // 头像（三级缓存：内存→本地文件→网络）
    QPixmap avatar;
    // 1. 首先检查内存缓存
    if (m_avatarCache.contains(avatarUrl)) {
        avatar = m_avatarCache[avatarUrl];
        qDebug() << "🔋 好友申请从内存缓存获取头像:" << fromUser;
    } 
    // 2. 然后检查本地文件缓存
    else {
        avatar = loadAvatarFromLocalCache(avatarUrl);
        if (!avatar.isNull()) {
            m_avatarCache[avatarUrl] = avatar; // 同时存入内存缓存
            qDebug() << "💾 好友申请从本地文件缓存加载头像成功:" << fromUser;
        } 
        // 3. 最后从网络下载
        else {
            qDebug() << "🌐 好友申请需要从网络下载头像:" << avatarUrl;
            downloadAvatar(avatarUrl); // 异步下载
        }
    }

    if (!avatar.isNull()) {
        painter->drawPixmap(option.rect.x() + 5, option.rect.y() + 5,
                            40, 40, avatar.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        qDebug() << "🖼️ 好友申请头像绘制完成:" << fromUser;
    } else {
        qDebug() << "⚠️ 好友申请头像为空，未绘制:" << fromUser;
    }

    // 文字
    QFont font = option.font;
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(option.state & QStyle::State_Selected ?
                        Qt::black : // 选中时文字黑色
                        option.palette.text().color());
    // 指定文字绘制的位置和内容fromUser（申请人的名字）
    painter->drawText(option.rect.x() + 50, option.rect.y() + 20, fromUser);

    font.setBold(false);
    painter->setFont(font);
    // 指定文字绘制的位置和内容message（申请人的验证消息）
    painter->drawText(option.rect.x() + 50, option.rect.y() + 40, message);

    // ========== 新增：绘制未处理请求的红点（status=0） ==========
    if (status == 0) {
        // 1. 红点参数（和按钮红点样式一致）
        int badgeSize = 18; // 红点直径
        int badgeX = option.rect.right() - badgeSize - 5; // 项右下角，右偏移5px
        int badgeY = option.rect.bottom() - badgeSize - 5; // 项右下角，下偏移5px
        QRect badgeRect(badgeX, badgeY, badgeSize, badgeSize);

        // 2. 绘制圆形红点背景
        painter->setRenderHint(QPainter::Antialiasing); // 抗锯齿，圆角更顺滑
        painter->setBrush(QColor("#FF3B30")); // 微信红
        painter->setPen(Qt::NoPen); // 无描边
        painter->drawEllipse(badgeRect); // 绘制圆形

        // 3. 绘制白色“1”文本（居中）
        painter->setPen(Qt::white);
        QFont badgeFont = font;
        badgeFont.setPixelSize(12); // 文字大小
        badgeFont.setBold(true);
        painter->setFont(badgeFont);

        // 文字居中计算
        QFontMetrics fm(badgeFont);
        QString text = "1";
        int textX = badgeRect.x() + (badgeRect.width() - fm.horizontalAdvance(text)) / 2;
        int textY = badgeRect.y() + (badgeRect.height() + fm.ascent() - fm.descent()) / 2;
        painter->drawText(textX, textY, text);
    }

    painter->restore();
}

QSize FriendRequestDelegate::sizeHint(const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QSize(option.rect.width(), 50);
}

void FriendRequestDelegate::downloadAvatar(const QString &url) const
{
    if (url.isEmpty()) return;
    m_netManager.get(QNetworkRequest(QUrl(url)));
}

void FriendRequestDelegate::onAvatarDownloaded(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "❌ 好友申请头像下载失败:" << reply->errorString() << "URL:" << reply->url().toString();
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    QPixmap avatar;
    if (avatar.loadFromData(data)) {
        QString url = reply->url().toString();
        m_avatarCache[url] = avatar; // 存入内存缓存
        saveAvatarToLocalCache(url, avatar); // 保存到本地文件缓存

        // 刷新视图
        if (parent() && parent()->inherits("QAbstractItemView")) {
            QAbstractItemView* view = qobject_cast<QAbstractItemView*>(parent());
            if (view) {
                view->viewport()->update();
            }
        }
        qDebug() << "✅ 好友申请头像下载并缓存完成:" << url;
    } else {
        qDebug() << "❌ 好友申请头像数据加载失败:" << reply->url().toString();
    }

    reply->deleteLater();
}
