// 懒加载实现：滚动到顶部时加载更多聊天记录
void MainWindow::onChatViewScrollValueChanged(int value)
{
    // 检查是否滚动到顶部且有更多消息可加载
    if (value == 0 && !m_isLoading && m_chatModel->hasMoreMessages()) {
        m_isLoading = true; // 设置加载状态，防止重复加载
        
        // 显示加载提示（在聊天视图底部添加加载中消息）
        ChatMessage loadingMsg;
        loadingMsg.content = "正在加载更多聊天记录...";
        loadingMsg.owner = ChatMessage::System;
        loadingMsg.type = ChatMessage::Text;
        loadingMsg.senderName = "系统";
        loadingMsg.avatarPath = "";
        loadingMsg.timestamp = QDateTime::currentDateTime();
        m_chatModel->addMessage(loadingMsg);
        
        // 自动滚动到底部显示加载提示
        QMetaObject::invokeMethod(this, [=] { 
            ui->chat_info_show->scrollToBottom();
        }, Qt::QueuedConnection);
        
        // 向后端发送获取历史消息的请求
        QJsonObject requestObj;
        requestObj["type"] = "get_chat_records";
        requestObj["from"] = m_currentUser;
        requestObj["to"] = m_selectedFriend;
        requestObj["pull_num"] = 10; // 每次加载10条历史消息
        
        // 获取当前最早消息的时间戳，用于向后端请求更早的消息
        if (m_chatModel->rowCount() > 0) {
            QVector<ChatMessage> messages = m_chatModel->messages();
            if (!messages.isEmpty()) {
                ChatMessage oldestMsg = messages.first();
                requestObj["time"] = oldestMsg.timestamp.toString("yyyy-MM-dd hh:mm:ss");
            }
        } else {
            requestObj["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        }
        
        // 发送WebSocket请求
        QJsonDocument doc(requestObj);
        sendWebSocketMessage(doc.toJson(QJsonDocument::Compact));
        qDebug() << "🔍 发送查询历史聊天记录请求：" << m_currentUser << " ↔ " << m_selectedFriend;
    }
}