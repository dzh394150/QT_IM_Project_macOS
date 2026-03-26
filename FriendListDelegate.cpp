#include "FriendListDelegate.h"
#include "FriendListModel.h"
#include "CacheUtils.h"
#include <QCryptographicHash>

FriendListDelegate::FriendListDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
    connect(&m_netManager, &QNetworkAccessManager::finished,
            this, &FriendListDelegate::onAvatarDownloaded);
    
    // 使用现有的CacheUtils类获取头像缓存目录
    m_avatarCacheDir = CacheUtils::getAvatarCacheDir();
    qDebug() << "使用现有头像缓存目录:" << m_avatarCacheDir;
}

// 生成头像本地缓存文件路径
QString FriendListDelegate::getAvatarCachePath(const QString &url) const
{
    QByteArray hash = QCryptographicHash::hash(url.toUtf8(), QCryptographicHash::Md5);
    return m_avatarCacheDir + "/" + hash.toHex() + ".jpg";
}

// 从本地缓存加载头像
QPixmap FriendListDelegate::loadAvatarFromLocalCache(const QString &url) const
{
    QString cachePath = getAvatarCachePath(url);
    QPixmap avatar;
    if (avatar.load(cachePath)) {
        qDebug() << "✅ 从本地缓存加载头像成功:" << cachePath;
        return avatar;
    }
    qDebug() << "❌ 从本地缓存加载头像失败:" << cachePath;
    return QPixmap();
}

// 保存头像到本地缓存
void FriendListDelegate::saveAvatarToLocalCache(const QString &url, const QPixmap &avatar) const
{
    QString cachePath = getAvatarCachePath(url);
    if (avatar.save(cachePath, "JPG", 80)) {
        qDebug() << "💾 头像已保存到本地缓存:" << cachePath;
    } else {
        qDebug() << "❌ 头像保存到本地缓存失败:" << cachePath;
    }
}

void FriendListDelegate::paint(QPainter *painter,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index) const
{
    // qDebug() << "\n🖌️ 执行FriendListDelegate::paint，行号：" << index.row();
    if (!index.isValid()) return;

    painter->save();

    // 背景
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, QColor(220, 220, 220)); // RGB 浅灰色
    } else {
        painter->fillRect(option.rect, option.palette.base());
    }

    QString username = index.data(FriendListModel::UsernameRole).toString();

    //【0126晚上未读消息无法显示修复逻辑开始】新增读取委托自身缓存的逻辑
    int unreadCount = m_unreadCountCache.value(username, 0);
    // qDebug() << "🚨 好友委托最终读取：" << username << "=" << unreadCount;
    //【0126晚上未读消息无法显示修复逻辑结束】

    QString avatarUrl = index.data(Qt::UserRole + 2).toString();
    //qDebug() << "📥 获取到头像URL:" << avatarUrl << "(用户名:" << username << ")";

    QPixmap avatar;
    // 1. 首先检查内存缓存
    if (m_avatarCache.contains(avatarUrl)) {
        avatar = m_avatarCache[avatarUrl];
        //qDebug() << "🔋 从内存缓存获取头像:" << username;
    } 
    // 2. 然后检查本地文件缓存
    else {
        avatar = loadAvatarFromLocalCache(avatarUrl);
        if (!avatar.isNull()) {
            m_avatarCache[avatarUrl] = avatar; // 同时存入内存缓存
            qDebug() << "💾 从本地文件缓存加载头像成功:" << username;
        } 
        // 3. 最后从网络下载
        else {
            qDebug() << "🌐 需要从网络下载头像:" << avatarUrl;
            downloadAvatar(avatarUrl);
        }
    }

    if (!avatar.isNull()) {
        painter->drawPixmap(option.rect.x() + 5, option.rect.y() + 5,
                            40, 40, avatar.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        //qDebug() << "🖼️ 绘制头像完成:" << username;
    } else {
        qDebug() << "⚠️ 头像为空，未绘制:" << username;
    }

    QFont font = option.font;
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(option.state & QStyle::State_Selected ?
                        Qt::black : // 选中时文字黑色
                        option.palette.text().color());
    painter->drawText(option.rect.x() + 50, option.rect.y() + 25, username);
    // 【0123白天未读消息逻辑开始：绘制未读红点（修正为右下角）】
    // 1. 获取当前好友的未读数量
    //【0126晚上未读消息无法显示修复逻辑开始】注释掉原有读取Model的逻辑
    // unreadCount = index.data(Qt::UserRole + 3).toInt(); // UnreadCountRole = Qt::UserRole +3
    //【0126晚上未读消息无法显示修复逻辑结束】
    // qDebug() << "🟢 好友Model返回未读数量：行号=" << index.row() << "，值=" << unreadCount;
    if (unreadCount > 0) { // 未读数量>0时才绘制红点
        // 2. 红点参数（微信风格：右下角，圆形，红色）
        int badgeMinSize = 18; // 基础红点直径（数字1-9时）
        int badgeWidth = badgeMinSize;
        QString badgeText;

        // 修正：右下角坐标计算（核心修改）
        int badgeX = option.rect.right() - badgeWidth - 5; // 项右侧向内偏移5px（和之前一致）
        int badgeY = option.rect.bottom() - badgeMinSize - 5; // 项底部向内偏移5px（原先是top+5，现在是bottom-尺寸-5）

        // 3. 处理未读文字（1-99显示数字，99+显示99+）
        if (unreadCount <= 99) {
            badgeText = QString::number(unreadCount);
            // 数字超过1位时，加宽红点（适配“10”、“99”）
            if (unreadCount >= 10) {
                badgeWidth = 24; // 两位数字加宽红点
                badgeX = option.rect.right() - badgeWidth - 5; // 重新计算X坐标（右侧向内偏移）
            }
        } else {
            badgeText = "99+";
            badgeWidth = 30; // 99+加宽红点
            badgeX = option.rect.right() - badgeWidth - 5; // 重新计算X坐标
        }

        // 4. 绘制圆形红点背景（抗锯齿）
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setBrush(QColor("#FF3B30")); // 微信红
        painter->setPen(Qt::NoPen); // 无描边
        QRect badgeRect(badgeX, badgeY, badgeWidth, badgeMinSize);
        painter->drawEllipse(badgeRect); // 绘制椭圆形（适配不同宽度）

        // 5. 绘制白色文字（居中）
        painter->setPen(Qt::white);
        QFont badgeFont = font;
        badgeFont.setPixelSize(12); // 文字大小
        badgeFont.setBold(true);
        painter->setFont(badgeFont);

        // 文字居中计算
        QFontMetrics fm(badgeFont);
        int textX = badgeRect.x() + (badgeRect.width() - fm.horizontalAdvance(badgeText)) / 2;
        int textY = badgeRect.y() + (badgeRect.height() + fm.ascent() - fm.descent()) / 2;
        painter->drawText(textX, textY, badgeText);
    }
    // 【0123白天未读消息逻辑结束】
    painter->restore();
}

QSize FriendListDelegate::sizeHint(const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QSize(option.rect.width(), 50);
}

void FriendListDelegate::downloadAvatar(const QString &url) const
{
    if (url.isEmpty()) return;
    m_netManager.get(QNetworkRequest(QUrl(url)));
}

void FriendListDelegate::onAvatarDownloaded(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "❌ 头像下载失败:" << reply->errorString() << "URL:" << reply->url().toString();
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    QPixmap avatar;
    if (avatar.loadFromData(data)) {
        QString url = reply->url().toString();
        m_avatarCache[url] = avatar; // 存入内存缓存
        saveAvatarToLocalCache(url, avatar); // 保存到本地文件缓存

        if (parent() && parent()->inherits("QAbstractItemView")) {
            static_cast<QAbstractItemView*>(parent())->viewport()->update();
        }
        qDebug() << "✅ 头像下载并缓存完成:" << url;
    } else {
        qDebug() << "❌ 头像数据加载失败:" << reply->url().toString();
    }

    reply->deleteLater();
}
