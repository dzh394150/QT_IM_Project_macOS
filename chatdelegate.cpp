#include "chatdelegate.h"
#include <QPainter>
#include <QTextDocument>
#include <QApplication>
#include <QDateTime>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QImage>
#include <QCryptographicHash>
#include <QFile>
#include <QFontMetrics>
#include <QTextOption>
#include <QRegularExpression>
#include <QDebug>
#include <QDir>
#include <chatmessage.h>

// 静态常量（保留上下边距，保证气泡美观）
const int ChatDelegate::AVATAR_SIZE = 40;
const int ChatDelegate::ITEM_MARGIN = 8;
const int ChatDelegate::BUBBLE_PADDING_H = 10;  // 左右内边距10px
const int ChatDelegate::BUBBLE_PADDING_V = 12;   // 上下内边距8px
const int ChatDelegate::BUBBLE_RADIUS = 4;
const int ChatDelegate::TRIANGLE_SIZE = 8;
const int ChatDelegate::TEXT_MAX_WIDTH = 350;
const int ChatDelegate::NAME_TIME_HEIGHT = 20;
const int ChatDelegate::NAME_HORIZONTAL_PAD = 5;

// 【0113晚上新增】预览图最大高度静态常量
const int ChatDelegate::PREVIEW_MAX_HEIGHT = 200;

// 【修复：类外定义并初始化静态常量，解决重复初始化报错】
const int ChatDelegate::RequestType_Avatar = 1;
const int ChatDelegate::RequestType_Preview = 2;

ChatDelegate::ChatDelegate(QObject *parent)
    : QStyledItemDelegate(parent),
    m_cacheDir(CacheUtils::getAvatarCacheDir()),
    m_previewCacheDir(CacheUtils::getFilePreviewCacheDir()),
    m_netManager(new QNetworkAccessManager(this))
{
    // 双重保障：检查并创建头像缓存目录
    qDebug() << "头像缓存路径" << m_cacheDir;
    QDir avatarDir(m_cacheDir);
    if (!avatarDir.exists()) {
        avatarDir.mkpath(m_cacheDir);
        qDebug() << "【ChatDelegate 兜底创建】头像缓存目录：" << m_cacheDir;
    }

    // 双重保障：检查并创建预览图缓存目录
    QDir previewDir(m_previewCacheDir);
    if (!previewDir.exists()) {
        previewDir.mkpath(m_previewCacheDir);
        qDebug() << "【ChatDelegate 兜底创建】预览图缓存目录：" << m_previewCacheDir;
    }

    // 原有信号绑定
    connect(m_netManager, &QNetworkAccessManager::finished,
            this, &ChatDelegate::onAvatarDownloaded);
    connect(m_netManager, &QNetworkAccessManager::finished,
            this, &ChatDelegate::onPreviewDownloaded);
}

void ChatDelegate::setSslConfiguration(const QSslConfiguration &config)
{
    m_sslConf = config;
}

QString ChatDelegate::getCachedAvatarPath(const QString &url) const
{
    QByteArray hash = QCryptographicHash::hash(url.toUtf8(), QCryptographicHash::Md5);
    return QString("%1/%2.png").arg(m_cacheDir, hash.toHex());
}

// 时间格式化（输出：月-日 时:分，如 01-17 15:30）
QString ChatDelegate::formatTime(const QString &rawTime) const
{
    // qDebug() << "原始时间数据：" << rawTime;
    // 第一步：优先使用QDateTime规范解析（yyyy-MM-dd hh:mm:ss），返回 MM-dd HH:mm
    QDateTime dt = QDateTime::fromString(rawTime, "yyyy-MM-dd hh:mm:ss");
    if (dt.isValid()) {
        // 核心修改：格式符改为 "MM-dd HH:mm"，对应 月-日 时:分（24小时制）
        return dt.toString("MM-dd HH:mm");
    }

    // 第二步：正则表达式兜底，优先提取「月-日 时:分」格式，再兼容仅「时:分」
    // 正则1：匹配 MM-dd HH:mm 格式（如 01-17 15:30、12-31 23:59）
    QRegularExpression reDateTime(R"((\d{2}-\d{2})\s+(\d{2}:\d{2}))");
    QRegularExpressionMatch matchDateTime = reDateTime.match(rawTime);
    if (matchDateTime.hasMatch()) {
        // 拼接月日和时分，返回 "MM-dd HH:mm" 格式
        QString monthDay = matchDateTime.captured(1);
        QString hourMinute = matchDateTime.captured(2);
        return QString("%1 %2").arg(monthDay, hourMinute);
    }

    // 正则2：兼容原有仅时分的格式（若无法提取月日，返回「未知月日 时分」，保证需求落地）
    QRegularExpression reTime(R"((\d{2}:\d{2}))");
    QRegularExpressionMatch matchTime = reTime.match(rawTime);
    if (matchTime.hasMatch()) {
        // 若无有效月日，补充「未知月日」兜底，避免缺失核心信息
        return QString("未知月日 %1").arg(matchTime.captured(1));
    }

    // 第三步：最终兜底，返回完整的「未知日期时间」
    return "未知日期时间";
}

// 【完整修复：paint方法（解决头像消失、预览图加载提示遮挡问题）】
void ChatDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // 1. 获取基础数据
    QString content = index.data(Qt::DisplayRole).toString();
    int owner = index.data(Qt::UserRole + 1).toInt();
    QString senderName = index.data(Qt::UserRole + 3).toString();
    QString avatarUrl = index.data(Qt::UserRole + 4).toString();
    QString rawTime = index.data(Qt::UserRole + 5).toString();
    bool isSelf = (owner == 0);

    // 2. 绘制背景
    painter->fillRect(opt.rect, opt.palette.base());

    // 3. 加载头像（添加兜底绘制）
    QPixmap avatar;
    QString cachePath = getCachedAvatarPath(avatarUrl);
    if (QFile::exists(cachePath)) {
        avatar.load(cachePath);
    } else {
        // 发起头像请求
        QNetworkRequest req{QUrl(avatarUrl)};
        req.setSslConfiguration(m_sslConf);
        req.setAttribute(QNetworkRequest::User, RequestType_Avatar);
        m_netManager->get(req);
    }

    // 【修复：头像为空时，绘制兜底灰色圆形占位图】
    if (avatar.isNull()) {
        avatar = QPixmap(AVATAR_SIZE, AVATAR_SIZE);
        avatar.fill(Qt::transparent);
        QPainter avatarPainter(&avatar);
        avatarPainter.setRenderHint(QPainter::Antialiasing, true);
        // 绘制灰色圆形背景
        avatarPainter.setBrush(QColor(200, 200, 200));
        avatarPainter.setPen(Qt::NoPen);
        avatarPainter.drawEllipse(avatar.rect());
        // 绘制用户名首字母
        if (!senderName.isEmpty()) {
            avatarPainter.setPen(QColor(100, 100, 100));
            avatarPainter.drawText(avatar.rect(), Qt::AlignCenter, senderName.left(1).toUpper());
        }
        avatarPainter.end();
    }
    avatar = avatar.scaled(AVATAR_SIZE, AVATAR_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // 4. 判断是否为文件消息，分支处理文本/文件渲染
    bool isFileMsg = isFileMessage(index);
    QSize contentSize;
    FileAttachment fileAttach;
    QPixmap previewPixmap;
    QString effectivePreviewUrl; // 有效预览图URL（含降级逻辑）

    if (isFileMsg) {
        // 4.1 提取文件附件信息
        QVariant fileAttachVar = index.data(Qt::UserRole + 6);
        if (fileAttachVar.isValid()) {
            fileAttach = fileAttachVar.value<FileAttachment>();
        }

        // 4.2 处理预览图URL（预览图为空则降级用原图）
        effectivePreviewUrl = fileAttach.previewUrl;
        if (effectivePreviewUrl.isEmpty()) {
            effectivePreviewUrl = fileAttach.originUrl;
        }

        // 4.3 加载预览图（缓存优先）
        QString previewCachePath = getCachedPreviewPath(effectivePreviewUrl);
        if (QFile::exists(previewCachePath)) {
            previewPixmap.load(previewCachePath);
        } else {
            if (!effectivePreviewUrl.isEmpty()) {
                downloadFilePreview(effectivePreviewUrl);
                // 【修复：仅在气泡内显示加载提示，不遮挡头像】
                painter->drawText(opt.rect.adjusted(
                                      BUBBLE_PADDING_H,
                                      BUBBLE_PADDING_V + NAME_TIME_HEIGHT,
                                      -BUBBLE_PADDING_H,
                                      -BUBBLE_PADDING_V
                                      ), Qt::AlignCenter, "正在加载预览图...");
            } else {
                painter->drawText(opt.rect.adjusted(
                                      BUBBLE_PADDING_H,
                                      BUBBLE_PADDING_V + NAME_TIME_HEIGHT,
                                      -BUBBLE_PADDING_H,
                                      -BUBBLE_PADDING_V
                                      ), Qt::AlignCenter, "无预览图");
            }
        }

        // 4.4 计算预览图尺寸（适配气泡）
        contentSize = calculatePreviewSize(effectivePreviewUrl, TEXT_MAX_WIDTH);
    } else {
        // 4.5 原有文本消息尺寸计算
        contentSize = calculateTextSize(content, TEXT_MAX_WIDTH);
    }

    // 5. 计算气泡尺寸（兼容文本/预览图）
    QSize bubbleSize(
        contentSize.width() + 2 * BUBBLE_PADDING_H,
        contentSize.height() + 2 * BUBBLE_PADDING_V
        );

    // 6. 头像位置（固定，无修改）
    QRect avatarRect;
    avatarRect.setSize(QSize(AVATAR_SIZE, AVATAR_SIZE));
    if (isSelf) {
        avatarRect.moveTo(
            opt.rect.right() - ITEM_MARGIN - AVATAR_SIZE,
            opt.rect.top() + ITEM_MARGIN + NAME_TIME_HEIGHT
            );
    } else {
        avatarRect.moveTo(
            opt.rect.left() + ITEM_MARGIN,
            opt.rect.top() + ITEM_MARGIN + NAME_TIME_HEIGHT
            );
    }

    // 7. 时间格式化+用户名绘制（固定，无修改）
    QString timeStr = formatTime(rawTime);
    QString nameTimeText = QString("%1  %2").arg(senderName, timeStr);
    QFontMetrics nameFm(QApplication::font());
    int nameTextWidth = nameFm.horizontalAdvance(nameTimeText);
    QRect nameTimeRect(0, 0, nameTextWidth, NAME_TIME_HEIGHT);
    if (isSelf) {
        nameTimeRect.moveTo(
            avatarRect.left() - nameTextWidth - NAME_HORIZONTAL_PAD,
            avatarRect.top() - NAME_TIME_HEIGHT
            );
    } else {
        nameTimeRect.moveTo(
            avatarRect.right() + NAME_HORIZONTAL_PAD,
            avatarRect.top() - NAME_TIME_HEIGHT
            );
    }

    // 8. 气泡位置计算（固定，无修改）
    QRect bubbleRect;
    bubbleRect.setSize(bubbleSize);
    int triangleY = avatarRect.center().y();
    if (isSelf) {
        bubbleRect.moveTo(
            avatarRect.left() - ITEM_MARGIN - bubbleSize.width(),
            avatarRect.top()
            );
    } else {
        bubbleRect.moveTo(
            avatarRect.right() + ITEM_MARGIN + 4,
            avatarRect.top()
            );
    }

    // 9. 文本/预览图绘制区域计算
    QRect contentRect = bubbleRect.adjusted(
        BUBBLE_PADDING_H,
        BUBBLE_PADDING_V,
        -BUBBLE_PADDING_H,
        -BUBBLE_PADDING_V
        );

    // 10. 绘制元素
    // 10.1 用户名+时间
    painter->setPen(QColor(100, 100, 100));
    painter->drawText(nameTimeRect, Qt::AlignCenter, nameTimeText);

    // 10.2 头像
    painter->drawPixmap(avatarRect, avatar);

    // 10.3 气泡+三角
    drawChatBubble(painter, bubbleRect, isSelf, triangleY);

    // 10.4 分支绘制文本/预览图
    painter->setPen(Qt::black);
    if (isFileMsg && !previewPixmap.isNull()) {
        // 绘制预览图（居中显示，保持宽高比）
        QPixmap scaledPreview = previewPixmap.scaled(contentRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QRect previewDrawRect = scaledPreview.rect();
        previewDrawRect.moveCenter(contentRect.center());
        painter->drawPixmap(previewDrawRect, scaledPreview);
    } else if (isFileMsg && previewPixmap.isNull() && !effectivePreviewUrl.isEmpty()) {
        // 预览图加载失败，绘制提示文本
        painter->drawText(contentRect, Qt::AlignCenter, "预览图加载中");
    } else if (isFileMsg && effectivePreviewUrl.isEmpty()) {
        painter->drawText(contentRect, Qt::AlignCenter, "无预览图");
    } else {
        // // 原有文本消息绘制逻辑
        // // QTextDocument是 QT 的「文本排版与渲染核心类」，相当于一个轻量级的 “文本编辑器内核”
        // QTextDocument doc;
        // // setPlainText()表示：传入的content是「纯文本」
        // doc.setPlainText(content);
        // // contentRect：是聊天气泡的「内容区域矩形」（气泡去掉左右上下内边距后的区域，也就是文本可以显示的区域
        // // setTextWidth()：给QTextDocument设置「文本排版的最大宽度约束」，即文本排版时，水平方向的最大可用宽度为contentRect的宽度；
        // doc.setTextWidth(contentRect.width());
        // // QTextOption是 QT 的「文本排版选项类」
        // QTextOption textOpt;
        // // Qt::AlignLeft：水平左对齐，Qt::AlignVCenter：垂直居中对齐
        // textOpt.setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

        // textOpt.setWrapMode(QTextOption::NoWrap);


        // // 将配置好的设置进行应用
        // doc.setDefaultTextOption(textOpt);

        // painter->save();
        // painter->translate(contentRect.topLeft() + QPoint(-3, -3));
        // doc.drawContents(painter);
        // painter->restore();

        // 替换原有QTextDocument绘制逻辑：纯手动实现换行，不依赖QT自带换行
        // 【0116鏖战两小时就只是解决了一个文本换行】
        // 这是因为QT自带的换行逻辑和屎一样，把人带进沟里一个半小时，后来自己写半小时搞定了
        QFontMetrics fm(QApplication::font());
        int maxContentWidth = contentRect.width();  // 气泡内容区域最大宽度（换行判断依据）
        QStringList totalDrawLines;  // 最终待绘制的所有行列表

        // 步骤1：先按\n拆分手动换行（保留你熟悉的逻辑）
        QStringList manualLines = content.split('\n');

        // 步骤2：对每一行手动拆分超长内容，生成可显示行
        for (const QString &manualLine : manualLines) {
            if (manualLine.isEmpty()) {
                // 空行直接保留（对应连续\n的场景）
                totalDrawLines.append("");
                continue;
            }

            int currentLineWidth = 0;  // 当前累加的像素宽度
            QString currentDrawLine;   // 当前正在构建的可显示行

            for (int i = 0; i < manualLine.length(); ++i) {
                QChar ch = manualLine.at(i);
                // 计算当前字符的像素宽度（单个字符的宽度）
                int chWidth = fm.horizontalAdvance(ch);

                // 判断：累加当前字符后是否超出最大宽度（预留少量冗余，避免紧贴边界）
                if (currentLineWidth + chWidth > maxContentWidth && !currentDrawLine.isEmpty()) {
                    // 超出宽度：将当前构建的行加入待绘制列表
                    totalDrawLines.append(currentDrawLine);
                    // 重置累加器，开始构建新行
                    currentDrawLine.clear();
                    currentLineWidth = 0;
                }

                // 累加字符和宽度
                currentDrawLine.append(ch);
                currentLineWidth += chWidth;
            }

            // 循环结束：将最后一行未完成的内容加入待绘制列表
            if (!currentDrawLine.isEmpty()) {
                totalDrawLines.append(currentDrawLine);
            }
        }

        // 步骤3：逐行绘制文本（手动控制每行位置，实现换行效果）
        painter->save();
        painter->setPen(Qt::black);  // 文本颜色（可根据需求调整）

        // 计算文本绘制的起始坐标（保持原有垂直居中+左对齐，贴合原有样式）
        int startX = contentRect.left();  // 水平左对齐，和原有逻辑一致
        // 垂直居中：总文本高度 = 总行数 × 行高，起始Y轴坐标微调实现垂直居中
        int totalTextHeight = totalDrawLines.size() * fm.height();
        int startY = contentRect.top() + (contentRect.height() - totalTextHeight) / 2;

        // 逐行绘制，每行间隔一个标准行高
        for (int i = 0; i < totalDrawLines.size(); ++i) {
            const QString &drawLine = totalDrawLines.at(i);
            // 计算当前行的绘制Y坐标
            int currentY = startY + i * fm.height();
            // 绘制当前行文本（左对齐，不使用任何QT换行功能）
            painter->drawText(QPoint(startX, currentY + fm.ascent()), drawLine);
        }

        painter->restore();
    }

    painter->restore();
}

QSize ChatDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // 判断是否为文件消息，适配预览图尺寸
    bool isFileMsg = isFileMessage(index);
    QSize contentSize;

    if (isFileMsg) {
        // 提取文件附件，计算预览图尺寸
        QVariant fileAttachVar = index.data(Qt::UserRole + 6);
        if (fileAttachVar.isValid()) {
            FileAttachment fileAttach = fileAttachVar.value<FileAttachment>();
            QString effectivePreviewUrl = fileAttach.previewUrl;
            if (effectivePreviewUrl.isEmpty()) {
                effectivePreviewUrl = fileAttach.originUrl;
            }
            contentSize = calculatePreviewSize(effectivePreviewUrl, TEXT_MAX_WIDTH);
        }
    } else {
        // 原有文本消息尺寸计算
        QString content = index.data(Qt::DisplayRole).toString();
        contentSize = calculateTextSize(content, TEXT_MAX_WIDTH);
    }

    int bubbleHeight = contentSize.height() + 2 * BUBBLE_PADDING_V;
    int totalHeight = NAME_TIME_HEIGHT + qMax(AVATAR_SIZE, bubbleHeight) + 2 * ITEM_MARGIN;
    return QSize(option.rect.width(), totalHeight);
}

// 【修复：添加错误处理的onAvatarDownloaded】
void ChatDelegate::onAvatarDownloaded(QNetworkReply *reply)
{
    int reqType = reply->request().attribute(QNetworkRequest::User, 0).toInt();
    if (reqType != RequestType_Avatar) {
        reply->deleteLater();
        return; // 不是头像请求，不处理
    }

    // 错误处理
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "头像请求失败：" << reply->errorString() << "URL：" << reply->url().toString();
        reply->deleteLater();
        return;
    }

    QString url = reply->url().toString();
    QByteArray data = reply->readAll();
    QImage image;
    if (image.loadFromData(data)) {
        QString cachePath = getCachedAvatarPath(url);
        if (image.save(cachePath, "PNG")) {
            QAbstractItemView *view = qobject_cast<QAbstractItemView*>(parent());
            if (view) {
                view->viewport()->update();
            }
        } else {
            qDebug() << "头像保存失败：" << cachePath;
        }
    } else {
        qDebug() << "头像数据损坏：" << url;
    }
    reply->deleteLater();
}

// ========== 核心修复3：三角与气泡无缝拼接（坐标精准匹配） ==========
void ChatDelegate::drawChatBubble(QPainter *painter, const QRect &bubbleRect, bool isSelf, int triangleY) const
{
    QPainterPath bubblePath;
    // 用RGB值填充自己发送的聊天气泡颜色
    QColor bubbleColor = isSelf ? QColor(154, 242, 109) : Qt::white;

    bubblePath.addRoundedRect(bubbleRect, BUBBLE_RADIUS, BUBBLE_RADIUS);

    QPointF p1, p2, p3;
    if (isSelf) {
        p1 = QPointF(bubbleRect.right() + TRIANGLE_SIZE, triangleY);
        // 核心修改：p2/p3 向气泡移1px（消除缝隙）
        p2 = QPointF(bubbleRect.right() + 1, triangleY - TRIANGLE_SIZE);
        p3 = QPointF(bubbleRect.right() + 1, triangleY + TRIANGLE_SIZE);
        bubblePath.addPolygon(QPolygonF({p2, p1, p3}));
    } else {
        p1 = QPointF(bubbleRect.left() - TRIANGLE_SIZE, triangleY);
        // 核心修改：p2/p3不移动
        p2 = QPointF(bubbleRect.left(), triangleY - TRIANGLE_SIZE);
        p3 = QPointF(bubbleRect.left(), triangleY + TRIANGLE_SIZE);
        bubblePath.addPolygon(QPolygonF({p2, p1, p3}));
    }
    bubblePath.closeSubpath();

    // 第二步：强制关闭画笔+确保填充无间隙
    painter->setPen(Qt::NoPen);          // 确保无轮廓线
    painter->setBrush(bubbleColor);     // 纯色填充
    painter->setRenderHint(QPainter::Antialiasing, false); // 临时关闭抗锯齿（消除边缘白边）
    painter->drawPath(bubblePath);
    painter->setRenderHint(QPainter::Antialiasing, true);  // 恢复抗锯齿（不影响其他绘制）
}

// 【0116晚上人工修改纯文本消息气泡高度宽度绘制逻辑】
QSize ChatDelegate::calculateTextSize(const QString &text, int maxWidth) const
{
    if (text.isEmpty()) return QSize(0, 0);

    // 创建QFontMetrics（字体度量）对象fm，该对象的核心作用是「获取指定字体的各种尺寸信息」（比如字符宽度、行高、字符间距等）
    QFontMetrics fm(QApplication::font());
    // 如果文本是"你好\n世界"，拆分后lines列表有两个元素："你好"和"世界"；如果文本中没有\n，则lines列表只有 1 个元素（整个文本）
    QStringList lines = text.split('\n');
    int maxLineWidth = 0;
    int totalHeight = 0;

    for (int i = 0; i < lines.size(); ++i) {
        const QString &line = lines.at(i);
        // 调用QFontMetrics的horizontalAdvance()方法，计算「当前行文本在不自动换行、单行完整显示时的总宽度（像素值）」；
        int lineWidth = fm.horizontalAdvance(line);
        maxLineWidth = qMax(maxLineWidth, lineWidth);
        // ===== 按照你的思路修改：超长行手动计算行数，短行保留原有逻辑 =====
        if (lineWidth > maxWidth) {
            // 1. 手动计算行数：总宽度 ÷ 最大宽度，向上取整（避免2.1行只算2行，导致高度不足）
            // qCeil()：QT的向上取整函数，需要包含 <cmath> 头文件
            int lineCount = qCeil(static_cast<qreal>(lineWidth) / static_cast<qreal>(maxWidth));
            // 2. 累加：行高 × 行数（完全按照你的思路：750宽→3行→累加3倍行高）
            totalHeight += fm.height() * lineCount;
        } else {
            // 保留：短行直接累加1倍行高，和原有逻辑一致
            totalHeight += fm.height();
        }
    }

    int finalWidth = qMin(maxLineWidth, maxWidth);
    return QSize(finalWidth, totalHeight);
}

// 【0113晚上新增功能：实现预览图缓存路径获取方法】
QString ChatDelegate::getCachedPreviewPath(const QString &previewUrl) const
{
    if (previewUrl.isEmpty()) {
        return QString();
    }
    QByteArray hash = QCryptographicHash::hash(previewUrl.toUtf8(), QCryptographicHash::Md5);
    return QString("%1/%2.png").arg(m_previewCacheDir, hash.toHex());
}

// 【0113晚上新增功能：实现预览图下载方法】
// 【0115晚上修正：新增正在下载的URL的记录】
void ChatDelegate::downloadFilePreview(const QString &previewUrl) const
{
    if (previewUrl.isEmpty()) {
        return;
    }

    // 步骤1：判断缓存是否已存在（避免重复请求）
    QString cachePath = getCachedPreviewPath(previewUrl);
    if (QFile::exists(cachePath)) {
        return;
    }

    // 步骤2：判断是否正在请求中（避免重复发起请求）
    if (m_pendingPreviewRequests.contains(previewUrl)) {
        return;
    }

    // 步骤3：加入正在请求集合
    m_pendingPreviewRequests.insert(previewUrl);

    QNetworkRequest req{QUrl(previewUrl)};
    req.setSslConfiguration(m_sslConf);
    req.setAttribute(QNetworkRequest::User, RequestType_Preview);
    req.setRawHeader("User-Agent", "Mozilla/5.0");
    req.setRawHeader("Accept", "image/*,*/*;q=0.8");
    QNetworkReply* reply_preview = m_netManager->get(req);

    connect(reply_preview, &QNetworkReply::sslErrors, this, [=](const QList<QSslError> &errors) {
        qDebug() << "大图请求SSL错误详情：";
        for (const QSslError &error : errors) {
            qDebug() << error.errorString();
        }
        reply_preview->ignoreSslErrors();
    });

    // 步骤4：修改lambda，请求完成后从正在请求集合中移除
    connect(reply_preview, &QNetworkReply::finished, this, [=]() {
        this->onPreviewDownloaded(reply_preview);
        // 核心：请求完成（无论成功/失败），都从正在请求集合中移除
        m_pendingPreviewRequests.remove(previewUrl);
    });
}

// 【0113晚上新增功能：实现预览图尺寸计算方法】
QSize ChatDelegate::calculatePreviewSize(const QString &previewUrl, int maxWidth) const
{
    if (previewUrl.isEmpty()) {
        return QSize(maxWidth, PREVIEW_MAX_HEIGHT);
    }

    QString cachePath = getCachedPreviewPath(previewUrl);
    QPixmap previewPixmap;
    if (QFile::exists(cachePath) && previewPixmap.load(cachePath)) {
        // 缩放尺寸：限制最大宽度和高度，保持宽高比
        return previewPixmap.scaled(maxWidth, PREVIEW_MAX_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation).size();
    }
    // 预览图未加载，返回默认尺寸
    return QSize(maxWidth, PREVIEW_MAX_HEIGHT);
}

// 【0113晚上新增功能：实现文件消息判断方法】
bool ChatDelegate::isFileMessage(const QModelIndex &index) const
{
    // 提取消息类型（UserRole + 2 为 TypeRole）
    int msgType = index.data(Qt::UserRole + 2).toInt();
    return (msgType == ChatMessage::Image || msgType == ChatMessage::File);
}

// 【修复：添加错误处理】
void ChatDelegate::onPreviewDownloaded(QNetworkReply *reply) const
{
    int reqType = reply->request().attribute(QNetworkRequest::User, 0).toInt();
    if (reqType != RequestType_Preview) {
        reply->deleteLater();
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "【预览图请求失败】" << reply->errorString() << "URL：" << reply->url().toString();
        reply->deleteLater();
        return;
    }

    QString url = reply->url().toString();
    QByteArray data = reply->readAll();
    QVariant contentType = reply->header(QNetworkRequest::ContentTypeHeader);
    QString contentTypeStr = contentType.toString().toLower();

    // 新增：过滤无效数据
    // 1. 数据长度过小（小于100字节，不可能是有效图片）
    if (data.size() < 100) {
        qDebug() << "【预览图无效数据】URL：" << url << " 数据长度过小：" << data.size();
        reply->deleteLater();
        return;
    }

    // 2. 检查数据是否为有效的图片格式（通过文件头判断）
    bool isValidImageData = false;
    if (data.size() >= 8) {
        // PNG文件头：89 50 4E 47 0D 0A 1A 0A
        if (data.startsWith("\x89PNG\r\n\x1A\n")) {
            isValidImageData = true;
        }
        // JPEG文件头：FF D8 FF
        else if (data.startsWith("\xFF\xD8\xFF")) {
            isValidImageData = true;
        }
        // GIF文件头：47 49 46 38
        else if (data.startsWith("GIF8")) {
            isValidImageData = true;
        }
    }

    // 如果Content-Type不是图片，但数据是有效的图片格式，仍然尝试加载
    if (!contentTypeStr.contains("image") &&
        !contentTypeStr.contains("jpg") &&
        !contentTypeStr.contains("png") &&
        !contentTypeStr.contains("gif")) {
        if (!isValidImageData) {
            qDebug() << "【预览图非图片格式】URL：" << url << " 内容类型：" << contentTypeStr;
            reply->deleteLater();
            return;
        } else {
            qDebug() << "【预览图Content-Type不正确但数据有效】URL：" << url << " 内容类型：" << contentTypeStr;
        }
    }

    QImage image;
    if (image.loadFromData(data)) {
        QString cachePath = getCachedPreviewPath(url);
        if (image.save(cachePath, "PNG")) {
            QAbstractItemView *view = qobject_cast<QAbstractItemView*>(parent());
            if (view) {
                view->viewport()->update();
            }
        } else {
            qDebug() << "【预览图保存失败】" << cachePath;
        }
    } else {
        qDebug() << "【预览图数据损坏】URL：" << url << " 数据长度：" << data.size() << " Content-Type:" << contentTypeStr;
    }
    reply->deleteLater();
}
