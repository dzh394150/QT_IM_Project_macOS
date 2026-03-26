# 核心模块（core+gui+widgets是GUI项目基础，websockets用于通信，sql用于本地数据库）
QT       += core gui widgets network websockets sql

# QT版本兼容（QT6自动满足，保留仅为兼容写法）
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# C++版本
CONFIG += c++17
QT_CONFIG -= no-pkg-config
# 禁用Qt扩展警告（可选，减少编译提示）
DEFINES += QT_DEPRECATED_WARNINGS

# 源文件列表
SOURCES += \
    AddFriends.cpp \
    CreateGroup.cpp \
    FileDownloader.cpp \
    FileTransferWorker.cpp \
    FileUtils.cpp \
    FriendListDelegate.cpp \
    FriendListModel.cpp \
    FriendRequestDelegate.cpp \
    FriendRequestModel.cpp \
    GroupListDelegate.cpp \
    GroupListModel.cpp \
    ImagePreviewDialog.cpp \
    LocalChatCache.cpp \
    cacheutils.cpp \
    chatdelegate.cpp \
    chatmodel.cpp \
    login.cpp \
    main.cpp \
    mainwindow.cpp \
    registerwidget.cpp

# 头文件列表
HEADERS += \
    AddFriends.h \
    CreateGroup.h \
    FileAttachment.h \
    FileChunkInfo.h \
    FileDownloader.h \
    FileTransferWorker.h \
    FileUtils.h \
    FriendListDelegate.h \
    FriendListModel.h \
    FriendRequestDelegate.h \
    FriendRequestModel.h \
    GroupListDelegate.h \
    GroupListModel.h \
    ImagePreviewDialog.h \
    LocalChatCache.h \
    cacheutils.h \
    chatdelegate.h \
    chatmessage.h \
    chatmodel.h \
    login.h \
    mainwindow.h \
    registerwidget.h

# UI界面文件列表
FORMS += \
    AddFriends.ui \
    CreateGroup.ui \
    ImagePreviewDialog.ui \
    login.ui \
    mainwindow.ui \
    registerwidget.ui

# 部署规则（默认保留）
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
