// chatmodel.h
#ifndef CHATMODEL_H
#define CHATMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include "chatmessage.h"
#include "FileAttachment.h"  // 【0113晚上新增功能：引入文件附件结构体头文件】

class ChatModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ChatModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void addMessage(const ChatMessage &msg);
    void prependMessages(const QVector<ChatMessage> &messages);
    void clearMessages();
    bool hasMoreMessages() const { return m_hasMoreMessages; }
    void setHasMoreMessages(bool hasMore) { m_hasMoreMessages = hasMore; }
    QVector<ChatMessage> messages() const { return m_messages; }
    void removeLastMessage();
    // 【0113晚上新增功能：声明文件附件角色（UserRole + 6）】
    static const int FileAttachRole = Qt::UserRole + 6;

private:
    QVector<ChatMessage> m_messages;
    bool m_hasMoreMessages = true; // 标记是否有更多历史消息可加载
};

#endif
