#include "GroupListDelegate.h"
#include <QApplication>
#include <GroupListModel.h>
#include <QRect>
#include <QString>
#include <QPixmap>
#include <QUrl>
#include <QNetworkRequest>
#include <QPalette>
#include <QAbstractItemView> // 新增：包含QAbstractItemView头文件
#include <QDebug> // 新增：调试输出

GroupListDelegate::GroupListDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
    // 连接网络请求完成信号（和FriendListDelegate一致）
    connect(&m_netManager, &QNetworkAccessManager::finished,
            this, &GroupListDelegate::onAvatarDownloaded);
    
    // 初始化本地头像缓存目录
    m_avatarCacheDir = CacheUtils::getAvatarCacheDir();
    qDebug() << "群聊列表头像缓存目录:" << m_avatarCacheDir;
}

GroupListDelegate::~GroupListDelegate()
{
}

// 生成头像本地缓存文件路径
QString GroupListDelegate::getAvatarCachePath(const QString &url) const
{
    QByteArray hash = QCryptographicHash::hash(url.toUtf8(), QCryptographicHash::Md5);
    return m_avatarCacheDir + "/" + hash.toHex() + ".jpg";
}

// 从本地缓存加载头像
QPixmap GroupListDelegate::loadAvatarFromLocalCache(const QString &url) const
{
    QString cachePath = getAvatarCachePath(url);
    QPixmap avatar;
    if (avatar.load(cachePath)) {
        qDebug() << "✅ 群聊从本地缓存加载头像成功:" << cachePath;
        return avatar;
    }
    qDebug() << "❌ 群聊从本地缓存加载头像失败:" << cachePath;
    return QPixmap();
}

// 保存头像到本地缓存
void GroupListDelegate::saveAvatarToLocalCache(const QString &url, const QPixmap &avatar) const
{
    QString cachePath = getAvatarCachePath(url);
    if (avatar.save(cachePath, "JPG", 80)) {
        qDebug() << "💾 群聊头像已保存到本地缓存:" << cachePath;
    } else {
        qDebug() << "❌ 群聊头像保存到本地缓存失败:" << cachePath;
    }
}

void GroupListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    qDebug() << "\n🖌️ 执行GroupListDelegate::paint，行号：" << index.row();
    if (!index.isValid()) return;

    painter->save();

    // ===== 1. 绘制背景（选中状态浅灰色，和好友列表一致）=====
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, QColor(220, 220, 220)); // 浅灰色背景
    } else {
        painter->fillRect(option.rect, option.palette.base()); // 默认背景
    }

    // ===== 2. 获取群聊数据（适配GroupListModel的角色）=====
    QString groupId = index.data(GroupListModel::GroupIdRole).toString(); // 【新增】获取群ID（缓存key）
    QString groupName = index.data(GroupListModel::GroupNameRole).toString();
    QString groupAvatarUrl = index.data(GroupListModel::GroupAvatarRole).toString();

    // 【0126睡前群聊未读红点逻辑修复逻辑开始】删除原有依赖Model的读取逻辑，改为读取委托缓存
    // ========== 关键修改：彻底绕过index.data() ==========
    // int unreadCount = 0;
    // 读取委托自身的缓存（群ID为key）
    int unreadCount = m_unreadCountCache.value(groupId, 0);
    qDebug() << "🚨 群聊委托最终读取：" << groupId << "=" << unreadCount;
    // 【0126睡前群聊未读红点逻辑修复逻辑结束】

    // ===== 3. 处理头像（三级缓存：内存→本地文件→网络）=====
    QPixmap avatar;
    // 1. 首先检查内存缓存
    if (m_avatarCache.contains(groupAvatarUrl)) {
        avatar = m_avatarCache[groupAvatarUrl];
        qDebug() << "🔋 群聊从内存缓存获取头像:" << groupName;
    } 
    // 2. 然后检查本地文件缓存
    else if (!groupAvatarUrl.isEmpty()) {
        avatar = loadAvatarFromLocalCache(groupAvatarUrl);
        if (!avatar.isNull()) {
            m_avatarCache[groupAvatarUrl] = avatar; // 同时存入内存缓存
            qDebug() << "💾 群聊从本地文件缓存加载头像成功:" << groupName;
        } 
        // 3. 最后从网络下载
        else {
            qDebug() << "🌐 群聊需要从网络下载头像:" << groupAvatarUrl;
            downloadAvatar(groupAvatarUrl);
        }
    }

    // ===== 4. 绘制头像（和好友列表坐标一致）=====
    if (!avatar.isNull()) {
        painter->drawPixmap(option.rect.x() + MARGIN, option.rect.y() + MARGIN,
                            AVATAR_SIZE, AVATAR_SIZE,
                            avatar.scaled(AVATAR_SIZE, AVATAR_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        qDebug() << "🖼️ 群聊头像绘制完成:" << groupName;
    } else {
        qDebug() << "⚠️ 群聊头像为空，未绘制:" << groupName;
    }

    // ===== 5. 绘制群名称（适配选中状态的文字颜色）=====
    QFont font = option.font;
    font.setBold(true); // 和好友列表一致，加粗文字
    painter->setFont(font);

    // 文字颜色：选中时黑色，否则默认文字色（和好友列表一致）
    painter->setPen(option.state & QStyle::State_Selected ?
                        Qt::black : option.palette.text().color());

    // 文字绘制坐标（和好友列表一致：x=50，y=25）
    painter->drawText(option.rect.x() + 50, option.rect.y() + 25, groupName);

    // 【0123白天未读消息逻辑开始：绘制群聊未读红点（右下角，微信风格）】
    // 【0126睡前群聊未读红点逻辑修复逻辑开始】注释掉原有读取Model的逻辑
    // 1. 获取当前群聊的未读数量
    // int unreadCount = index.data(GroupListModel::UnreadCountRole).toInt();
    // 【0126睡前群聊未读红点逻辑修复逻辑结束】

    qDebug() << "🟢 群聊Model缓存值：行号=" << index.row() << "，群ID=" << groupId << "，缓存未读数量=" << unreadCount;
    if (unreadCount > 0) { // 未读数量>0时才绘制红点
        // 2. 红点参数（和好友列表完全一致）
        int badgeMinSize = 18; // 基础红点直径（数字1-9时）
        int badgeWidth = badgeMinSize;
        QString badgeText;

        // 右下角坐标计算（和好友列表逻辑一致）
        int badgeX = option.rect.right() - badgeWidth - 5; // 右侧向内偏移5px
        int badgeY = option.rect.bottom() - badgeMinSize - 5; // 底部向内偏移5px

        // 3. 处理未读文字（1-99显示数字，99+显示99+）
        if (unreadCount <= 99) {
            badgeText = QString::number(unreadCount);
            // 数字超过1位时，加宽红点（适配“10”、“99”）
            if (unreadCount >= 10) {
                badgeWidth = 24; // 两位数字加宽红点
                badgeX = option.rect.right() - badgeWidth - 5; // 重新计算X坐标
            }
        } else {
            badgeText = "99+";
            badgeWidth = 30; // 99+加宽红点
            badgeX = option.rect.right() - badgeWidth - 5; // 重新计算X坐标
        }

        // 4. 绘制圆形红点背景（抗锯齿，微信红）
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

QSize GroupListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    // 返回固定高度（和好友列表一致）
    return QSize(option.rect.width(), ITEM_HEIGHT);
}

// ===== 头像下载逻辑（和FriendListDelegate一致）=====
void GroupListDelegate::downloadAvatar(const QString &url) const
{
    if (url.isEmpty()) return;
    m_netManager.get(QNetworkRequest(QUrl(url)));
}

// ===== 头像下载完成槽函数（修正类型转换错误）=====
void GroupListDelegate::onAvatarDownloaded(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "❌ 群聊头像下载失败:" << reply->errorString() << "URL:" << reply->url().toString();
        reply->deleteLater();
        return;
    }

    // 读取下载的头像数据
    QByteArray data = reply->readAll();
    QPixmap avatar;
    if (avatar.loadFromData(data)) {
        // 缓存头像（URL -> 图片）
        QString url = reply->url().toString();
        m_avatarCache[url] = avatar; // 存入内存缓存
        saveAvatarToLocalCache(url, avatar); // 保存到本地文件缓存

        // 修正：用qobject_cast安全转换，增加空指针检查
        if (parent()) {
            QAbstractItemView* view = qobject_cast<QAbstractItemView*>(parent());
            if (view) {
                view->viewport()->update();
            }
        }
        qDebug() << "✅ 群聊头像下载并缓存完成:" << url;
    } else {
        qDebug() << "❌ 群聊头像数据加载失败:" << reply->url().toString();
    }

    reply->deleteLater();
}
