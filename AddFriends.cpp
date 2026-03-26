#include "AddFriends.h"
#include "ui_AddFriends.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QUrlQuery>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QImage>
#include <QPixmap>

AddFriends::AddFriends(QNetworkAccessManager *netManager,
                       const QSslConfiguration &sslConf,
                       const QString &currentUser,
                       QWidget *parent)
    : QWidget(parent),
    ui(new Ui::AddFriends),
    m_netManager(netManager),
    m_sslConf(sslConf),
    m_currentUser(currentUser),
    m_isDragging(false)
{
    ui->setupUi(this);

    // 允许鼠标跟踪
    setMouseTracking(true);

    // 绑定按钮
    connect(ui->SearchFriend, &QPushButton::clicked,
            this, &AddFriends::onSearchFriendClicked);
    connect(ui->AddthisFriend, &QPushButton::clicked,
            this, &AddFriends::onAddFriendClicked);
    connect(m_netManager, &QNetworkAccessManager::sslErrors,
            this, &AddFriends::onSslErrors);
    connect(ui->close_botton, &QPushButton::clicked,
            this, &AddFriends::close);

    // 初始隐藏不需要的控件
    ui->AddthisFriend->hide();
    ui->CheckInfo->hide();
    ui->FriendStatus->hide();
    ui->NewFriendPhoto->hide();
    ui->NewFriendNumber->hide();
}

AddFriends::~AddFriends()
{
    delete ui;
}

void AddFriends::onSearchFriendClicked()
{
    QString targetUser = ui->WriteFriendNumber->text().trimmed();
    if (targetUser.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入用户名");
        return;
    }

    QUrl url("https://www.singchat.chat/friend/search");
    QUrlQuery query;
    query.addQueryItem("username", targetUser);
    query.addQueryItem("current_user", m_currentUser);
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setSslConfiguration(m_sslConf);

    QNetworkReply *reply = m_netManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [=]() {
        if (reply->error() != QNetworkReply::NoError) {
            QMessageBox::critical(this, "错误", "搜索失败: " + reply->errorString());
            reply->deleteLater();
            return;
        }

        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            QMessageBox::critical(this, "错误", "返回数据格式错误");
            reply->deleteLater();
            return;
        }

        QJsonObject obj = doc.object();
        bool exists = obj["exists"].toBool();
        if (!exists) {
            QMessageBox::information(this, "提示", "用户不存在");
            reply->deleteLater();
            return;
        }

        bool isFriend = obj["is_friend"].toBool();
        QJsonObject userObj = obj["user"].toObject();
        QString avatarUrl = userObj["avatar"].toString();

        // 显示账号标签
        ui->NewFriendNumber->setText(userObj["username"].toString());
        ui->NewFriendNumber->show();

        // 加载并显示头像
        QNetworkRequest avatarRequest(avatarUrl);
        avatarRequest.setSslConfiguration(m_sslConf);
        QNetworkReply *avatarReply = m_netManager->get(avatarRequest);
        connect(avatarReply, &QNetworkReply::finished, this, [=]() {
            onAvatarDownloaded(avatarReply);
        });

        if (isFriend) {
            ui->FriendStatus->setText("你们已经是好友");
            ui->FriendStatus->show();
            ui->AddthisFriend->hide();
            ui->CheckInfo->hide();
        } else {
            ui->FriendStatus->hide();
            ui->AddthisFriend->show();
            ui->CheckInfo->show();
        }

        reply->deleteLater();
    });
}

void AddFriends::onAddFriendClicked()
{
    QString targetUser = ui->NewFriendNumber->text().trimmed();
    if (targetUser.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先搜索用户");
        return;
    }

    QString message = ui->CheckInfo->text().trimmed();

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart fromPart;
    fromPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QVariant("form-data; name=\"from\""));
    fromPart.setBody(m_currentUser.toUtf8());

    QHttpPart toPart;
    toPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                     QVariant("form-data; name=\"to\""));
    toPart.setBody(targetUser.toUtf8());

    QHttpPart messagePart;
    messagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                          QVariant("form-data; name=\"message\""));
    messagePart.setBody(message.toUtf8());

    multiPart->append(fromPart);
    multiPart->append(toPart);
    multiPart->append(messagePart);

    QUrl url("https://www.singchat.chat/friend/request");
    QNetworkRequest request(url);
    request.setSslConfiguration(m_sslConf);

    QNetworkReply *reply = m_netManager->post(request, multiPart);
    multiPart->setParent(reply);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        if (reply->error() != QNetworkReply::NoError) {
            QMessageBox::critical(this, "错误", "发送请求失败: " + reply->errorString());
            reply->deleteLater();
            return;
        }

        QByteArray data = reply->readAll();
        QString response = QString::fromUtf8(data);
        QMessageBox::information(this, "提示", response);
        reply->deleteLater();
    });
}

void AddFriends::onAvatarDownloaded(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QImage image;
        image.loadFromData(reply->readAll());
        if (!image.isNull()) {
            ui->NewFriendPhoto->setPixmap(QPixmap::fromImage(image).scaled(
                ui->NewFriendPhoto->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            ui->NewFriendPhoto->show(); // 加载成功后显示头像标签
        }
    }
    reply->deleteLater();
}

void AddFriends::onSslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    QList<QSslError> ignoreErrors;
    for (const QSslError &error : errors) {
        if (error.error() == QSslError::HostNameMismatch ||
            error.error() == QSslError::SelfSignedCertificate ||
            error.error() == QSslError::CertificateUntrusted) {
            ignoreErrors << error;
        }
    }
    if (!ignoreErrors.isEmpty()) {
        reply->ignoreSslErrors(ignoreErrors);
    }
}

// ================= 拖动实现 =================
void AddFriends::mousePressEvent(QMouseEvent *event)
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

void AddFriends::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPosition().toPoint() - m_dragStartPosition);
        event->accept(); // 消费事件
        return;
    }

    QWidget::mouseMoveEvent(event);
}

void AddFriends::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        event->accept();
    }

    QWidget::mouseReleaseEvent(event);
}
// ===========================================

