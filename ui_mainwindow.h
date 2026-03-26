/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "mainwindow.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout_2;
    QWidget *widget1;
    QVBoxLayout *verticalLayout;
    QWidget *widget1_1;
    QPushButton *userAvatar;
    QWidget *widget1_2;
    QPushButton *btnLookFriend;
    QPushButton *btnStartChat;
    QPushButton *groupchat;
    QSpacerItem *verticalSpacer1_2_and_1_3;
    QWidget *widget1_3;
    QStackedWidget *stackedWidget2;
    QWidget *stackedWidget2_chat;
    QVBoxLayout *verticalLayout_2;
    QWidget *widget2_chat_1;
    QLineEdit *lineEdit2_chat_2;
    QWidget *widget2_chat_2;
    QVBoxLayout *verticalLayout_4;
    QListView *person_list;
    QWidget *stackedWidget2_info;
    QVBoxLayout *verticalLayout_3;
    QWidget *widget2_info_1;
    QLineEdit *lineEdit2_chat_1;
    QPushButton *btnAddFriends;
    QWidget *widget2_info_2;
    QVBoxLayout *verticalLayout_7;
    QListView *friendRequest_list;
    QWidget *stackedWidget2_group;
    QVBoxLayout *verticalLayout_8;
    QWidget *foundarea;
    QPushButton *btnCreateGroup;
    QLineEdit *groupfoud;
    QWidget *groupwidget;
    QVBoxLayout *verticalLayout_9;
    QListView *group_list;
    QStackedWidget *stackedWidget3;
    QWidget *stackedWidget3_chat;
    QVBoxLayout *verticalLayout_5;
    QWidget *widget3_1;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QWidget *widget3_1_1;
    QHBoxLayout *horizontalLayout_9;
    QPushButton *minimize_botton;
    QPushButton *maximize_botton;
    QPushButton *close_botton;
    QWidget *widget3_2;
    QHBoxLayout *horizontalLayout_6;
    QWidget *widget3_2_2;
    QHBoxLayout *horizontalLayout_7;
    QLabel *person_name;
    QSpacerItem *horizontalSpacer3_2;
    QWidget *widget3_2_1;
    QWidget *widget3_3;
    QHBoxLayout *horizontalLayout_8;
    QListView *chat_info_show;
    QWidget *widget3_4;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer3_4;
    QWidget *widget3_1_2;
    QPushButton *filetran;
    QPushButton *getimage;
    QPushButton *chatrecord;
    QWidget *widget3_5;
    QHBoxLayout *horizontalLayout_3;
    ChatInputTextEdit *chat_input;
    QWidget *widget3_6;
    QHBoxLayout *horizontalLayout_5;
    QWidget *widgetfile;
    QHBoxLayout *horizontalLayout_12;
    QProgressBar *fileprogress;
    QSpacerItem *horizontalSpacer3_6;
    QWidget *widget3_6_1;
    QPushButton *send_botton;
    QWidget *NewFriendInfoWidget;
    QVBoxLayout *verticalLayout_6;
    QWidget *widget3_7;
    QHBoxLayout *horizontalLayout_10;
    QSpacerItem *horizontalSpacer_2;
    QWidget *widget3_1_3;
    QHBoxLayout *horizontalLayout_11;
    QPushButton *minimize_botton_2;
    QPushButton *maximize_botton_2;
    QPushButton *close_botton_2;
    QWidget *FriendInfoShowwidget;
    QLabel *photoArea;
    QLabel *nameArea;
    QPushButton *btnAgree;
    QPushButton *btnRefuse;
    QLabel *cheakinfoArea;
    QLabel *Friend_status;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMinimumSize(QSize(600, 600));
        MainWindow->setStyleSheet(QString::fromUtf8("/* MainWindow\351\200\217\346\230\216 */\n"
"MainWindow {\n"
"    background: transparent !important; \n"
"    border: none !important;\n"
"} \n"
"\n"
""));
        MainWindow->setAnimated(true);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName("centralWidget");
        sizePolicy.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy);
        centralWidget->setMinimumSize(QSize(600, 600));
        centralWidget->setMaximumSize(QSize(16777215, 16777215));
        centralWidget->setStyleSheet(QString::fromUtf8("/* QT 6.10\351\200\202\351\205\215\357\274\232centralWidget\351\200\217\346\230\216\357\274\214\351\201\277\345\205\215\351\201\256\346\214\241 */\n"
"centralWidget {\n"
"    background: transparent !important;\n"
"    border: none !important;\n"
"}"));
        horizontalLayout_2 = new QHBoxLayout(centralWidget);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        widget1 = new QWidget(centralWidget);
        widget1->setObjectName("widget1");
        sizePolicy.setHeightForWidth(widget1->sizePolicy().hasHeightForWidth());
        widget1->setSizePolicy(sizePolicy);
        widget1->setMinimumSize(QSize(60, 0));
        widget1->setMaximumSize(QSize(60, 16777215));
        widget1->setStyleSheet(QString::fromUtf8("#widget1 {\n"
"    /* \346\265\205\347\201\260\350\211\262\350\203\214\346\231\257\357\274\210\345\276\256\344\277\241PC\347\253\257\345\270\270\347\224\250\346\265\205\347\201\260\357\274\211 */\n"
"    background-color: #F0F0F0 !important;\n"
"    /* \346\267\261\347\201\260\350\211\2620.5px\350\276\271\346\241\206\357\274\210\345\242\236\345\274\272\350\247\206\350\247\211\345\214\272\345\210\206\357\274\211 */\n"
"    border: 0.5px solid #D6D6D6 !important;\n"
"    /* \345\267\246\344\270\212\350\247\222\345\234\206\350\247\2228px\357\274\210\344\277\235\346\214\201\345\216\237\345\247\213\345\200\274\357\274\214\344\270\215\303\2275\357\274\211 */\n"
"    border-top-left-radius: 8px !important;\n"
"    /* \345\267\246\344\270\213\350\247\222\345\234\206\350\247\2228px\357\274\210\344\277\235\346\214\201\345\216\237\345\247\213\345\200\274\357\274\211 */\n"
"    border-bottom-left-radius: 8px !important;\n"
"    /* \345\217\263\344\270\212/\345\217\263\344\270\213\344\270\215\350\256\276\345\234\206\350\247"
                        "\222\357\274\214\351\273\230\350\256\244\345\260\226\350\247\222\357\274\214\346\227\240\351\234\200\351\242\235\345\244\226\345\206\231 */\n"
"    /* \345\274\272\345\210\266\346\230\276\347\244\272\357\274\214\346\216\222\351\231\244\351\232\220\350\227\217/\351\200\217\346\230\216\345\271\262\346\211\260 */\n"
"    visibility: visible !important;\n"
"    opacity: 1 !important;\n"
"}"));
        verticalLayout = new QVBoxLayout(widget1);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        widget1_1 = new QWidget(widget1);
        widget1_1->setObjectName("widget1_1");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(widget1_1->sizePolicy().hasHeightForWidth());
        widget1_1->setSizePolicy(sizePolicy1);
        widget1_1->setMinimumSize(QSize(60, 80));
        widget1_1->setMaximumSize(QSize(60, 80));
        userAvatar = new QPushButton(widget1_1);
        userAvatar->setObjectName("userAvatar");
        userAvatar->setEnabled(true);
        userAvatar->setGeometry(QRect(10, 20, 40, 40));
        userAvatar->setStyleSheet(QString::fromUtf8("#userAvatar {\n"
"    /* \345\237\272\347\241\200\346\240\267\345\274\217\357\274\232\346\227\240\350\276\271\346\241\206 + \345\233\233\350\247\222\345\234\206\350\247\222 + \351\200\217\346\230\216\350\203\214\346\231\257 */\n"
"    border: none !important;                /* \345\275\273\345\272\225\351\232\220\350\227\217\350\276\271\346\241\206 */\n"
"    border-radius: 8px !important;          /* \345\233\233\350\247\222\345\234\206\350\247\222\357\274\210\345\200\274\345\217\257\346\214\211\351\234\200\350\260\203\346\225\264\357\274\214\345\246\2026px/10px\357\274\211 */\n"
"    background: transparent !important;     /* \345\210\235\345\247\213\351\200\217\346\230\216 */\n"
"\n"
"    outline: none !important;               /* \345\216\273\346\216\211\347\204\246\347\202\271\346\241\206\357\274\210QT\351\273\230\350\256\244\347\202\271\345\207\273\345\220\216\344\274\232\346\234\211\357\274\211 */\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\202\254\346\265\256\357\274\232\346\257\224#F0F0F0\347\250\215"
                        "\346\267\261\347\232\204\347\201\260\350\211\262\357\274\210#EBEBEB\357\274\214\350\277\207\346\270\241\350\207\252\347\204\266\357\274\211 */\n"
"#userAvatar:hover {\n"
"    background-color: #DCDCDC !important;\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\214\211\344\270\213\357\274\232\346\233\264\346\267\261\344\270\200\347\202\271\347\232\204\347\201\260\350\211\262\357\274\210#DCDCDC\357\274\214\350\264\250\346\204\237\346\262\211\347\250\263\357\274\211 */\n"
"#userAvatar:pressed {\n"
"    background-color: #CCCCCC !important;\n"
"}"));

        verticalLayout->addWidget(widget1_1);

        widget1_2 = new QWidget(widget1);
        widget1_2->setObjectName("widget1_2");
        sizePolicy1.setHeightForWidth(widget1_2->sizePolicy().hasHeightForWidth());
        widget1_2->setSizePolicy(sizePolicy1);
        widget1_2->setMinimumSize(QSize(60, 360));
        widget1_2->setMaximumSize(QSize(60, 360));
        btnLookFriend = new QPushButton(widget1_2);
        btnLookFriend->setObjectName("btnLookFriend");
        btnLookFriend->setEnabled(true);
        btnLookFriend->setGeometry(QRect(10, 60, 40, 40));
        btnLookFriend->setStyleSheet(QString::fromUtf8("QPushButton#btnLookFriend {\n"
"    width: 40px !important;\n"
"    height: 40px !important;\n"
"    border-image: url(btnimage/\350\201\224\347\263\273\344\272\272\345\270\270\346\200\201.png) 0 0 0 0 stretch stretch;\n"
"    border: none;\n"
"    background-color: transparent;\n"
"    padding: 0px !important;\n"
"}\n"
"\n"
"QPushButton#btnLookFriend:checked {\n"
"    border-image: url(btnimage/\350\201\224\347\263\273\344\272\272\346\214\211\344\270\213.png) 0 0 0 0 stretch stretch !important;\n"
"}"));
        btnLookFriend->setCheckable(true);
        btnStartChat = new QPushButton(widget1_2);
        btnStartChat->setObjectName("btnStartChat");
        btnStartChat->setEnabled(true);
        btnStartChat->setGeometry(QRect(10, 10, 40, 40));
        btnStartChat->setStyleSheet(QString::fromUtf8("QPushButton#btnStartChat {\n"
"    width: 40px !important;\n"
"    height: 40px !important;\n"
"    border-image: url(btnimage/chat\345\270\270\346\200\201.png) 0 0 0 0 stretch stretch;\n"
"    border: none;\n"
"    background-color: transparent;\n"
"    padding: 0px !important;\n"
"}\n"
"\n"
"QPushButton#btnStartChat:checked {\n"
"    border-image: url(btnimage/chat\351\200\211\344\270\255.png) 0 0 0 0 stretch stretch !important;\n"
"}"));
        btnStartChat->setCheckable(true);
        groupchat = new QPushButton(widget1_2);
        groupchat->setObjectName("groupchat");
        groupchat->setEnabled(true);
        groupchat->setGeometry(QRect(10, 110, 40, 40));
        groupchat->setStyleSheet(QString::fromUtf8("QPushButton#groupchat {\n"
"    width: 40px !important;\n"
"    height: 40px !important;\n"
"    border-image: url(btnimage/\347\276\244\350\201\212\345\270\270\346\200\201.png) 0 0 0 0 stretch stretch;\n"
"    border: none;\n"
"    background-color: transparent;\n"
"    padding: 0px !important;\n"
"}\n"
"\n"
"QPushButton#groupchat:checked {\n"
"    border-image: url(btnimage/\347\276\244\350\201\212\351\200\211\344\270\255.png) 0 0 0 0 stretch stretch !important;\n"
"}"));
        groupchat->setCheckable(true);

        verticalLayout->addWidget(widget1_2);

        verticalSpacer1_2_and_1_3 = new QSpacerItem(20, 47, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer1_2_and_1_3);

        widget1_3 = new QWidget(widget1);
        widget1_3->setObjectName("widget1_3");
        widget1_3->setMinimumSize(QSize(60, 110));
        widget1_3->setMaximumSize(QSize(60, 110));

        verticalLayout->addWidget(widget1_3);


        horizontalLayout_2->addWidget(widget1);

        stackedWidget2 = new QStackedWidget(centralWidget);
        stackedWidget2->setObjectName("stackedWidget2");
        sizePolicy.setHeightForWidth(stackedWidget2->sizePolicy().hasHeightForWidth());
        stackedWidget2->setSizePolicy(sizePolicy);
        stackedWidget2->setMinimumSize(QSize(200, 0));
        stackedWidget2->setMaximumSize(QSize(200, 1677715));
        stackedWidget2->setStyleSheet(QString::fromUtf8("#stackedWidget2 {\n"
"    /* \346\265\205\347\201\260\350\211\262\350\203\214\346\231\257\357\274\210\344\277\235\347\225\231\345\216\237\346\234\211\357\274\211 */\n"
"    background-color: #FFFFFF !important;\n"
"    /* \344\273\205\346\230\276\347\244\272\344\270\212\344\270\213\350\276\271\346\241\206\357\274\214\345\267\246\345\217\263\346\227\240\350\276\271\346\241\206\357\274\210\346\240\270\345\277\203\344\277\256\346\224\271\357\274\211 */\n"
"    border-top: 0.5px solid #D6D6D6 !important;    /* \344\270\212\350\276\271\346\241\206\357\274\2320.5px\346\267\261\347\201\260\345\256\236\345\277\203 */\n"
"    border-bottom: 0.5px solid #D6D6D6 !important; /* \344\270\213\350\276\271\346\241\206\357\274\2320.5px\346\267\261\347\201\260\345\256\236\345\277\203 */\n"
"    border-left: none !important;                /* \345\267\246\350\276\271\346\241\206\357\274\232\351\232\220\350\227\217 */\n"
"    border-right: none !important;               /* \345\217\263\350\276\271\346\241\206\357\274\232\351\232\220"
                        "\350\227\217 */\n"
"    /* \345\274\272\345\210\266\346\230\276\347\244\272\357\274\214\346\216\222\351\231\244\351\232\220\350\227\217/\351\200\217\346\230\216\345\271\262\346\211\260\357\274\210\344\277\235\347\225\231\345\216\237\346\234\211\357\274\211 */\n"
"    visibility: visible !important;\n"
"    opacity: 1 !important;\n"
"}"));
        stackedWidget2_chat = new QWidget();
        stackedWidget2_chat->setObjectName("stackedWidget2_chat");
        stackedWidget2_chat->setStyleSheet(QString::fromUtf8(""));
        verticalLayout_2 = new QVBoxLayout(stackedWidget2_chat);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        widget2_chat_1 = new QWidget(stackedWidget2_chat);
        widget2_chat_1->setObjectName("widget2_chat_1");
        sizePolicy1.setHeightForWidth(widget2_chat_1->sizePolicy().hasHeightForWidth());
        widget2_chat_1->setSizePolicy(sizePolicy1);
        widget2_chat_1->setMinimumSize(QSize(200, 80));
        widget2_chat_1->setMaximumSize(QSize(200, 80));
        lineEdit2_chat_2 = new QLineEdit(widget2_chat_1);
        lineEdit2_chat_2->setObjectName("lineEdit2_chat_2");
        lineEdit2_chat_2->setGeometry(QRect(10, 30, 181, 30));
        lineEdit2_chat_2->setStyleSheet(QString::fromUtf8("#lineEdit2_chat_2 {\n"
"    /* \342\221\240 \345\275\273\345\272\225\351\232\220\350\227\217\350\276\271\346\241\206\357\274\210\345\214\205\346\213\254\351\273\230\350\256\244\347\232\204\347\204\246\347\202\271\350\276\271\346\241\206\357\274\211 */\n"
"    border: none !important;\n"
"    outline: none !important;\n"
"    \n"
"    /* \342\221\241 \345\234\206\350\247\222\347\237\251\345\275\242\357\274\21012px\346\230\257\345\276\256\344\277\241\350\276\223\345\205\245\346\241\206\345\270\270\347\224\250\345\234\206\350\247\222\357\274\214\345\217\257\346\214\211\351\234\200\350\260\203\346\225\264\357\274\211 */\n"
"    border-radius: 12px !important;\n"
"    \n"
"    /* \342\221\242 \345\272\225\350\211\262\350\256\276\344\270\272#F0F0F0\347\201\260\350\211\262 */\n"
"    background-color: #F0F0F0 !important;\n"
"    \n"
"    /* \344\274\230\345\214\226\357\274\232\345\206\205\350\276\271\350\267\235\357\274\214\351\201\277\345\205\215\346\226\207\345\255\227\350\264\264\345\234\206\350\247\222\350\276\271"
                        "\347\274\230\357\274\210\345\217\257\351\200\211\344\275\206\346\216\250\350\215\220\357\274\211 */\n"
"    padding: 6px 12px !important;\n"
"}"));

        verticalLayout_2->addWidget(widget2_chat_1);

        widget2_chat_2 = new QWidget(stackedWidget2_chat);
        widget2_chat_2->setObjectName("widget2_chat_2");
        verticalLayout_4 = new QVBoxLayout(widget2_chat_2);
        verticalLayout_4->setSpacing(0);
        verticalLayout_4->setObjectName("verticalLayout_4");
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        person_list = new QListView(widget2_chat_2);
        person_list->setObjectName("person_list");
        person_list->setStyleSheet(QString::fromUtf8("#person_list {\n"
"    /* 1. \345\275\273\345\272\225\346\227\240\350\276\271\346\241\206\357\274\210\346\240\270\345\277\203\351\234\200\346\261\202\357\274\211 */\n"
"    border: none !important;          /* \345\216\273\346\216\211\345\210\227\350\241\250\345\216\237\347\224\237\350\276\271\346\241\206 */\n"
"    outline: none !important;         /* \345\216\273\346\216\211\347\204\246\347\202\271\351\253\230\344\272\256\346\241\206 */\n"
"    \n"
"    /* 2. \345\276\256\344\277\241\351\243\216\346\240\274\350\203\214\346\231\257\350\211\262 */\n"
"    background-color: #FFFFFF !important; /* \345\222\214\345\276\256\344\277\241\346\266\210\346\201\257\345\210\227\350\241\250\350\203\214\346\231\257\344\270\200\350\207\264 */\n"
"    \n"
"    /* 3. \345\210\227\350\241\250\351\241\271\351\227\264\350\267\235\357\274\210\345\217\257\351\200\211\357\274\214\346\250\241\346\213\237\345\276\256\344\277\241\345\210\227\350\241\250\351\241\271\351\227\264\350\267\235\357\274\211 */\n"
"    spacing: 2px !important"
                        ";\n"
"}\n"
"\n"
"/* \347\276\216\345\214\226\346\273\232\345\212\250\346\235\241\357\274\210\345\276\256\344\277\241\347\252\204\346\273\232\345\212\250\346\235\241\357\274\214\345\217\257\351\200\211\344\275\206\346\216\250\350\215\220\357\274\211 */\n"
"#person_list QScrollBar:vertical {\n"
"    background: transparent !important; /* \346\273\232\345\212\250\346\235\241\350\203\214\346\231\257\351\200\217\346\230\216 */\n"
"    width: 6px !important;              /* \347\252\204\346\273\232\345\212\250\346\235\241\357\274\214\350\264\264\345\220\210\345\276\256\344\277\241 */\n"
"}\n"
"#person_list QScrollBar::handle:vertical {\n"
"    background-color: #D1D1D1 !important; /* \346\273\232\345\212\250\346\235\241\346\273\221\345\235\227\351\242\234\350\211\262 */\n"
"    border-radius: 3px !important;        /* \346\273\221\345\235\227\345\234\206\350\247\222 */\n"
"}\n"
"#person_list QScrollBar::handle:vertical:hover {\n"
"    background-color: #BFBFBF !important; /* \351\274\240\346\240\207\346\202\254\346\265"
                        "\256\346\273\221\345\235\227\345\212\240\346\267\261 */\n"
"}\n"
"#person_list QScrollBar::add-page:vertical,\n"
"#person_list QScrollBar::sub-page:vertical {\n"
"    background: transparent !important; /* \346\273\232\345\212\250\346\235\241\347\251\272\347\231\275\345\214\272\345\237\237\351\200\217\346\230\216 */\n"
"}"));

        verticalLayout_4->addWidget(person_list);


        verticalLayout_2->addWidget(widget2_chat_2);

        stackedWidget2->addWidget(stackedWidget2_chat);
        stackedWidget2_info = new QWidget();
        stackedWidget2_info->setObjectName("stackedWidget2_info");
        verticalLayout_3 = new QVBoxLayout(stackedWidget2_info);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        widget2_info_1 = new QWidget(stackedWidget2_info);
        widget2_info_1->setObjectName("widget2_info_1");
        sizePolicy1.setHeightForWidth(widget2_info_1->sizePolicy().hasHeightForWidth());
        widget2_info_1->setSizePolicy(sizePolicy1);
        widget2_info_1->setMinimumSize(QSize(200, 80));
        widget2_info_1->setMaximumSize(QSize(200, 80));
        lineEdit2_chat_1 = new QLineEdit(widget2_info_1);
        lineEdit2_chat_1->setObjectName("lineEdit2_chat_1");
        lineEdit2_chat_1->setGeometry(QRect(10, 30, 141, 30));
        lineEdit2_chat_1->setStyleSheet(QString::fromUtf8("#lineEdit2_chat_1 {\n"
"    /* \342\221\240 \345\275\273\345\272\225\351\232\220\350\227\217\350\276\271\346\241\206\357\274\210\345\214\205\346\213\254\351\273\230\350\256\244\347\232\204\347\204\246\347\202\271\350\276\271\346\241\206\357\274\211 */\n"
"    border: none !important;\n"
"    outline: none !important;\n"
"    \n"
"    /* \342\221\241 \345\234\206\350\247\222\347\237\251\345\275\242\357\274\21012px\346\230\257\345\276\256\344\277\241\350\276\223\345\205\245\346\241\206\345\270\270\347\224\250\345\234\206\350\247\222\357\274\214\345\217\257\346\214\211\351\234\200\350\260\203\346\225\264\357\274\211 */\n"
"    border-radius: 12px !important;\n"
"    \n"
"    /* \342\221\242 \345\272\225\350\211\262\350\256\276\344\270\272#F0F0F0\347\201\260\350\211\262 */\n"
"    background-color: #F0F0F0 !important;\n"
"    \n"
"    /* \344\274\230\345\214\226\357\274\232\345\206\205\350\276\271\350\267\235\357\274\214\351\201\277\345\205\215\346\226\207\345\255\227\350\264\264\345\234\206\350\247\222\350\276\271"
                        "\347\274\230\357\274\210\345\217\257\351\200\211\344\275\206\346\216\250\350\215\220\357\274\211 */\n"
"    padding: 6px 12px !important;\n"
"}"));
        btnAddFriends = new QPushButton(widget2_info_1);
        btnAddFriends->setObjectName("btnAddFriends");
        btnAddFriends->setEnabled(true);
        btnAddFriends->setGeometry(QRect(160, 30, 30, 30));
        QFont font;
        font.setFamilies({QString::fromUtf8("\347\255\211\347\272\277")});
        font.setPointSize(24);
        font.setBold(false);
        btnAddFriends->setFont(font);
        btnAddFriends->setStyleSheet(QString::fromUtf8("#btnAddFriends {\n"
"    /* \345\237\272\347\241\200\346\240\267\345\274\217\357\274\232\346\227\240\350\276\271\346\241\206 + \345\233\233\350\247\222\345\234\206\350\247\222 + \351\200\217\346\230\216\350\203\214\346\231\257 */\n"
"    border: none !important;                /* \345\275\273\345\272\225\351\232\220\350\227\217\350\276\271\346\241\206 */\n"
"    border-radius: 8px !important;          /* \345\233\233\350\247\222\345\234\206\350\247\222\357\274\210\345\200\274\345\217\257\346\214\211\351\234\200\350\260\203\346\225\264\357\274\214\345\246\2026px/10px\357\274\211 */\n"
"    background-color: #F0F0F0 !important;\n"
"    outline: none !important;               /* \345\216\273\346\216\211\347\204\246\347\202\271\346\241\206\357\274\210QT\351\273\230\350\256\244\347\202\271\345\207\273\345\220\216\344\274\232\346\234\211\357\274\211 */\n"
"    /* \346\226\260\345\242\236\357\274\232\350\256\276\347\275\256\345\255\227\344\275\223\351\242\234\350\211\262\357\274\210#666666\346\230\257\346\257\224#F0F"
                        "0F0\346\267\261\345\276\227\345\244\232\347\232\204\347\201\260\350\211\262\357\274\214\345\217\257\350\257\273\346\200\247\344\275\263\357\274\211 */\n"
"    color: #AAAAAA !important;\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\202\254\346\265\256\357\274\232\346\257\224#F0F0F0\347\250\215\346\267\261\347\232\204\347\201\260\350\211\262\357\274\214\345\255\227\344\275\223\351\242\234\350\211\262\344\277\235\346\214\201\344\270\215\345\217\230 */\n"
"#btnAddFriends:hover {\n"
"    background-color: #DCDCDC !important;\n"
"    /* \346\230\276\345\274\217\347\273\247\346\211\277/\344\277\235\347\225\231\347\233\270\345\220\214\345\255\227\344\275\223\351\242\234\350\211\262\357\274\214\351\230\262\346\255\242\350\242\253\345\205\266\344\273\226\346\240\267\345\274\217\350\246\206\347\233\226 */\n"
"    color: #AAAAAA !important;\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\214\211\344\270\213\357\274\232\346\233\264\346\267\261\344\270\200\347\202\271\347\232\204\347\201\260\350\211\262\357\274\214\345\255"
                        "\227\344\275\223\351\242\234\350\211\262\344\277\235\346\214\201\344\270\215\345\217\230 */\n"
"#btnAddFriends:pressed {\n"
"    background-color: #CCCCCC !important;\n"
"    /* \346\230\276\345\274\217\347\273\247\346\211\277/\344\277\235\347\225\231\347\233\270\345\220\214\345\255\227\344\275\223\351\242\234\350\211\262\357\274\214\351\230\262\346\255\242\350\242\253\345\205\266\344\273\226\346\240\267\345\274\217\350\246\206\347\233\226 */\n"
"    color: #AAAAAA !important;\n"
"}"));

        verticalLayout_3->addWidget(widget2_info_1);

        widget2_info_2 = new QWidget(stackedWidget2_info);
        widget2_info_2->setObjectName("widget2_info_2");
        verticalLayout_7 = new QVBoxLayout(widget2_info_2);
        verticalLayout_7->setSpacing(0);
        verticalLayout_7->setObjectName("verticalLayout_7");
        verticalLayout_7->setContentsMargins(0, 0, 0, 0);
        friendRequest_list = new QListView(widget2_info_2);
        friendRequest_list->setObjectName("friendRequest_list");
        friendRequest_list->setStyleSheet(QString::fromUtf8("#friendRequest_list {\n"
"    /* 1. \345\275\273\345\272\225\346\227\240\350\276\271\346\241\206\357\274\210\346\240\270\345\277\203\351\234\200\346\261\202\357\274\211 */\n"
"    border: none !important;          /* \345\216\273\346\216\211\345\210\227\350\241\250\345\216\237\347\224\237\350\276\271\346\241\206 */\n"
"    outline: none !important;         /* \345\216\273\346\216\211\347\204\246\347\202\271\351\253\230\344\272\256\346\241\206 */\n"
"    \n"
"    /* 2. \345\276\256\344\277\241\351\243\216\346\240\274\350\203\214\346\231\257\350\211\262 */\n"
"    background-color: #FFFFFF !important; /* \345\222\214\345\276\256\344\277\241\346\266\210\346\201\257\345\210\227\350\241\250\350\203\214\346\231\257\344\270\200\350\207\264 */\n"
"    \n"
"    /* 3. \345\210\227\350\241\250\351\241\271\351\227\264\350\267\235\357\274\210\345\217\257\351\200\211\357\274\214\346\250\241\346\213\237\345\276\256\344\277\241\345\210\227\350\241\250\351\241\271\351\227\264\350\267\235\357\274\211 */\n"
"    spacing: 2px !im"
                        "portant;\n"
"}\n"
"\n"
"/* \347\276\216\345\214\226\346\273\232\345\212\250\346\235\241\357\274\210\345\276\256\344\277\241\347\252\204\346\273\232\345\212\250\346\235\241\357\274\214\345\217\257\351\200\211\344\275\206\346\216\250\350\215\220\357\274\211 */\n"
"#friendRequest_list QScrollBar:vertical {\n"
"    background: transparent !important; /* \346\273\232\345\212\250\346\235\241\350\203\214\346\231\257\351\200\217\346\230\216 */\n"
"    width: 6px !important;              /* \347\252\204\346\273\232\345\212\250\346\235\241\357\274\214\350\264\264\345\220\210\345\276\256\344\277\241 */\n"
"}\n"
"#friendRequest_list QScrollBar::handle:vertical {\n"
"    background-color: #D1D1D1 !important; /* \346\273\232\345\212\250\346\235\241\346\273\221\345\235\227\351\242\234\350\211\262 */\n"
"    border-radius: 3px !important;        /* \346\273\221\345\235\227\345\234\206\350\247\222 */\n"
"}\n"
"#friendRequest_list QScrollBar::handle:vertical:hover {\n"
"    background-color: #BFBFBF !important; /* \351\274\240\346"
                        "\240\207\346\202\254\346\265\256\346\273\221\345\235\227\345\212\240\346\267\261 */\n"
"}\n"
"#friendRequest_list QScrollBar::add-page:vertical,\n"
"#friendRequest_list QScrollBar::sub-page:vertical {\n"
"    background: transparent !important; /* \346\273\232\345\212\250\346\235\241\347\251\272\347\231\275\345\214\272\345\237\237\351\200\217\346\230\216 */\n"
"}"));

        verticalLayout_7->addWidget(friendRequest_list);


        verticalLayout_3->addWidget(widget2_info_2);

        stackedWidget2->addWidget(stackedWidget2_info);
        stackedWidget2_group = new QWidget();
        stackedWidget2_group->setObjectName("stackedWidget2_group");
        verticalLayout_8 = new QVBoxLayout(stackedWidget2_group);
        verticalLayout_8->setSpacing(0);
        verticalLayout_8->setObjectName("verticalLayout_8");
        verticalLayout_8->setContentsMargins(0, 0, 0, 0);
        foundarea = new QWidget(stackedWidget2_group);
        foundarea->setObjectName("foundarea");
        foundarea->setMinimumSize(QSize(200, 80));
        foundarea->setMaximumSize(QSize(200, 80));
        btnCreateGroup = new QPushButton(foundarea);
        btnCreateGroup->setObjectName("btnCreateGroup");
        btnCreateGroup->setEnabled(true);
        btnCreateGroup->setGeometry(QRect(160, 30, 30, 30));
        btnCreateGroup->setFont(font);
        btnCreateGroup->setStyleSheet(QString::fromUtf8("#btnCreateGroup {\n"
"    /* \345\237\272\347\241\200\346\240\267\345\274\217\357\274\232\346\227\240\350\276\271\346\241\206 + \345\233\233\350\247\222\345\234\206\350\247\222 + \351\200\217\346\230\216\350\203\214\346\231\257 */\n"
"    border: none !important;                /* \345\275\273\345\272\225\351\232\220\350\227\217\350\276\271\346\241\206 */\n"
"    border-radius: 8px !important;          /* \345\233\233\350\247\222\345\234\206\350\247\222\357\274\210\345\200\274\345\217\257\346\214\211\351\234\200\350\260\203\346\225\264\357\274\214\345\246\2026px/10px\357\274\211 */\n"
"    background-color: #F0F0F0 !important;\n"
"    outline: none !important;               /* \345\216\273\346\216\211\347\204\246\347\202\271\346\241\206\357\274\210QT\351\273\230\350\256\244\347\202\271\345\207\273\345\220\216\344\274\232\346\234\211\357\274\211 */\n"
"    /* \346\226\260\345\242\236\357\274\232\350\256\276\347\275\256\345\255\227\344\275\223\351\242\234\350\211\262\357\274\210#666666\346\230\257\346\257\224#F0"
                        "F0F0\346\267\261\345\276\227\345\244\232\347\232\204\347\201\260\350\211\262\357\274\214\345\217\257\350\257\273\346\200\247\344\275\263\357\274\211 */\n"
"    color: #AAAAAA !important;\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\202\254\346\265\256\357\274\232\346\257\224#F0F0F0\347\250\215\346\267\261\347\232\204\347\201\260\350\211\262\357\274\214\345\255\227\344\275\223\351\242\234\350\211\262\344\277\235\346\214\201\344\270\215\345\217\230 */\n"
"#btnCreateGroup:hover {\n"
"    background-color: #DCDCDC !important;\n"
"    /* \346\230\276\345\274\217\347\273\247\346\211\277/\344\277\235\347\225\231\347\233\270\345\220\214\345\255\227\344\275\223\351\242\234\350\211\262\357\274\214\351\230\262\346\255\242\350\242\253\345\205\266\344\273\226\346\240\267\345\274\217\350\246\206\347\233\226 */\n"
"    color: #AAAAAA !important;\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\214\211\344\270\213\357\274\232\346\233\264\346\267\261\344\270\200\347\202\271\347\232\204\347\201\260\350\211\262\357\274\214\345\255"
                        "\227\344\275\223\351\242\234\350\211\262\344\277\235\346\214\201\344\270\215\345\217\230 */\n"
"#btnCreateGroup:pressed {\n"
"    background-color: #CCCCCC !important;\n"
"    /* \346\230\276\345\274\217\347\273\247\346\211\277/\344\277\235\347\225\231\347\233\270\345\220\214\345\255\227\344\275\223\351\242\234\350\211\262\357\274\214\351\230\262\346\255\242\350\242\253\345\205\266\344\273\226\346\240\267\345\274\217\350\246\206\347\233\226 */\n"
"    color: #AAAAAA !important;\n"
"}"));
        groupfoud = new QLineEdit(foundarea);
        groupfoud->setObjectName("groupfoud");
        groupfoud->setGeometry(QRect(10, 30, 141, 30));
        groupfoud->setStyleSheet(QString::fromUtf8("#groupfoud {\n"
"    /* \342\221\240 \345\275\273\345\272\225\351\232\220\350\227\217\350\276\271\346\241\206\357\274\210\345\214\205\346\213\254\351\273\230\350\256\244\347\232\204\347\204\246\347\202\271\350\276\271\346\241\206\357\274\211 */\n"
"    border: none !important;\n"
"    outline: none !important;\n"
"    \n"
"    /* \342\221\241 \345\234\206\350\247\222\347\237\251\345\275\242\357\274\21012px\346\230\257\345\276\256\344\277\241\350\276\223\345\205\245\346\241\206\345\270\270\347\224\250\345\234\206\350\247\222\357\274\214\345\217\257\346\214\211\351\234\200\350\260\203\346\225\264\357\274\211 */\n"
"    border-radius: 12px !important;\n"
"    \n"
"    /* \342\221\242 \345\272\225\350\211\262\350\256\276\344\270\272#F0F0F0\347\201\260\350\211\262 */\n"
"    background-color: #F0F0F0 !important;\n"
"    \n"
"    /* \344\274\230\345\214\226\357\274\232\345\206\205\350\276\271\350\267\235\357\274\214\351\201\277\345\205\215\346\226\207\345\255\227\350\264\264\345\234\206\350\247\222\350\276\271\347\274"
                        "\230\357\274\210\345\217\257\351\200\211\344\275\206\346\216\250\350\215\220\357\274\211 */\n"
"    padding: 6px 12px !important;\n"
"}"));

        verticalLayout_8->addWidget(foundarea);

        groupwidget = new QWidget(stackedWidget2_group);
        groupwidget->setObjectName("groupwidget");
        verticalLayout_9 = new QVBoxLayout(groupwidget);
        verticalLayout_9->setSpacing(0);
        verticalLayout_9->setObjectName("verticalLayout_9");
        verticalLayout_9->setContentsMargins(0, 0, 0, 0);
        group_list = new QListView(groupwidget);
        group_list->setObjectName("group_list");
        group_list->setStyleSheet(QString::fromUtf8("#group_list {\n"
"    /* 1. \345\275\273\345\272\225\346\227\240\350\276\271\346\241\206\357\274\210\346\240\270\345\277\203\351\234\200\346\261\202\357\274\211 */\n"
"    border: none !important;          /* \345\216\273\346\216\211\345\210\227\350\241\250\345\216\237\347\224\237\350\276\271\346\241\206 */\n"
"    outline: none !important;         /* \345\216\273\346\216\211\347\204\246\347\202\271\351\253\230\344\272\256\346\241\206 */\n"
"    \n"
"    /* 2. \345\276\256\344\277\241\351\243\216\346\240\274\350\203\214\346\231\257\350\211\262 */\n"
"    background-color: #FFFFFF !important; /* \345\222\214\345\276\256\344\277\241\346\266\210\346\201\257\345\210\227\350\241\250\350\203\214\346\231\257\344\270\200\350\207\264 */\n"
"    \n"
"    /* 3. \345\210\227\350\241\250\351\241\271\351\227\264\350\267\235\357\274\210\345\217\257\351\200\211\357\274\214\346\250\241\346\213\237\345\276\256\344\277\241\345\210\227\350\241\250\351\241\271\351\227\264\350\267\235\357\274\211 */\n"
"    spacing: 2px !important;"
                        "\n"
"}\n"
"\n"
"/* \347\276\216\345\214\226\346\273\232\345\212\250\346\235\241\357\274\210\345\276\256\344\277\241\347\252\204\346\273\232\345\212\250\346\235\241\357\274\214\345\217\257\351\200\211\344\275\206\346\216\250\350\215\220\357\274\211 */\n"
"#group_list QScrollBar:vertical {\n"
"    background: transparent !important; /* \346\273\232\345\212\250\346\235\241\350\203\214\346\231\257\351\200\217\346\230\216 */\n"
"    width: 6px !important;              /* \347\252\204\346\273\232\345\212\250\346\235\241\357\274\214\350\264\264\345\220\210\345\276\256\344\277\241 */\n"
"}\n"
"#group_list QScrollBar::handle:vertical {\n"
"    background-color: #D1D1D1 !important; /* \346\273\232\345\212\250\346\235\241\346\273\221\345\235\227\351\242\234\350\211\262 */\n"
"    border-radius: 3px !important;        /* \346\273\221\345\235\227\345\234\206\350\247\222 */\n"
"}\n"
"#group_list QScrollBar::handle:vertical:hover {\n"
"    background-color: #BFBFBF !important; /* \351\274\240\346\240\207\346\202\254\346\265\256"
                        "\346\273\221\345\235\227\345\212\240\346\267\261 */\n"
"}\n"
"#group_list QScrollBar::add-page:vertical,\n"
"#group_list QScrollBar::sub-page:vertical {\n"
"    background: transparent !important; /* \346\273\232\345\212\250\346\235\241\347\251\272\347\231\275\345\214\272\345\237\237\351\200\217\346\230\216 */\n"
"}"));

        verticalLayout_9->addWidget(group_list);


        verticalLayout_8->addWidget(groupwidget);

        stackedWidget2->addWidget(stackedWidget2_group);

        horizontalLayout_2->addWidget(stackedWidget2);

        stackedWidget3 = new QStackedWidget(centralWidget);
        stackedWidget3->setObjectName("stackedWidget3");
        sizePolicy.setHeightForWidth(stackedWidget3->sizePolicy().hasHeightForWidth());
        stackedWidget3->setSizePolicy(sizePolicy);
        stackedWidget3->setStyleSheet(QString::fromUtf8("#stackedWidget3 {\n"
"    /* \346\265\205\347\201\260\350\211\262\350\203\214\346\231\257\357\274\210\345\276\256\344\277\241PC\347\253\257\345\270\270\347\224\250\346\265\205\347\201\260\357\274\211 */\n"
"    background-color: #F5F5F5 !important;\n"
"    /* \346\267\261\347\201\260\350\211\2621px\350\276\271\346\241\206\357\274\210\345\242\236\345\274\272\350\247\206\350\247\211\345\214\272\345\210\206\357\274\211 */\n"
"    border: 0.5px solid #D6D6D6 !important;\n"
"    /* \345\217\263\344\270\212\350\247\222\345\234\206\350\247\2228px\357\274\210\344\277\235\346\214\201\345\216\237\345\247\213\345\200\274\357\274\214\344\270\215\303\2275\357\274\211 */\n"
"    border-top-right-radius: 8px !important;\n"
"    /* \345\217\263\344\270\213\350\247\222\345\234\206\350\247\2228px\357\274\210\344\277\235\346\214\201\345\216\237\345\247\213\345\200\274\357\274\211 */\n"
"    border-bottom-right-radius: 8px !important;\n"
"    /* \345\217\263\344\270\212/\345\217\263\344\270\213\344\270\215\350\256\276\345\234\206"
                        "\350\247\222\357\274\214\351\273\230\350\256\244\345\260\226\350\247\222\357\274\214\346\227\240\351\234\200\351\242\235\345\244\226\345\206\231 */\n"
"    /* \345\274\272\345\210\266\346\230\276\347\244\272\357\274\214\346\216\222\351\231\244\351\232\220\350\227\217/\351\200\217\346\230\216\345\271\262\346\211\260 */\n"
"    visibility: visible !important;\n"
"    opacity: 1 !important;\n"
"}"));
        stackedWidget3_chat = new QWidget();
        stackedWidget3_chat->setObjectName("stackedWidget3_chat");
        verticalLayout_5 = new QVBoxLayout(stackedWidget3_chat);
        verticalLayout_5->setSpacing(0);
        verticalLayout_5->setObjectName("verticalLayout_5");
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        widget3_1 = new QWidget(stackedWidget3_chat);
        widget3_1->setObjectName("widget3_1");
        widget3_1->setMinimumSize(QSize(0, 30));
        widget3_1->setMaximumSize(QSize(16777215, 30));
        horizontalLayout = new QHBoxLayout(widget3_1);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer = new QSpacerItem(385, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        widget3_1_1 = new QWidget(widget3_1);
        widget3_1_1->setObjectName("widget3_1_1");
        sizePolicy.setHeightForWidth(widget3_1_1->sizePolicy().hasHeightForWidth());
        widget3_1_1->setSizePolicy(sizePolicy);
        widget3_1_1->setMinimumSize(QSize(120, 30));
        widget3_1_1->setMaximumSize(QSize(120, 30));
        horizontalLayout_9 = new QHBoxLayout(widget3_1_1);
        horizontalLayout_9->setSpacing(0);
        horizontalLayout_9->setObjectName("horizontalLayout_9");
        horizontalLayout_9->setContentsMargins(0, 0, 0, 0);
        minimize_botton = new QPushButton(widget3_1_1);
        minimize_botton->setObjectName("minimize_botton");
        minimize_botton->setEnabled(true);
        minimize_botton->setMinimumSize(QSize(0, 30));
        minimize_botton->setMaximumSize(QSize(16777215, 30));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("\345\256\213\344\275\223")});
        font1.setPointSize(18);
        minimize_botton->setFont(font1);
        minimize_botton->setStyleSheet(QString::fromUtf8("#minimize_botton {\n"
"    /* \345\237\272\347\241\200\346\240\267\345\274\217\357\274\232\346\227\240\350\276\271\346\241\206 + \345\233\233\350\247\222\345\234\206\350\247\222 + \351\200\217\346\230\216\350\203\214\346\231\257 */\n"
"    border: none !important;                /* \345\275\273\345\272\225\351\232\220\350\227\217\350\276\271\346\241\206 */\n"
"\n"
"    background: transparent !important;     /* \345\210\235\345\247\213\351\200\217\346\230\216 */\n"
"\n"
"    outline: none !important;               /* \345\216\273\346\216\211\347\204\246\347\202\271\346\241\206\357\274\210QT\351\273\230\350\256\244\347\202\271\345\207\273\345\220\216\344\274\232\346\234\211\357\274\211 */\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\202\254\346\265\256\357\274\232\346\257\224#F0F0F0\347\250\215\346\267\261\347\232\204\347\201\260\350\211\262\357\274\210#EBEBEB\357\274\214\350\277\207\346\270\241\350\207\252\347\204\266\357\274\211 */\n"
"#minimize_botton:hover {\n"
"    background-color: #DCDCDC !important;\n"
""
                        "}\n"
"\n"
"/* \351\274\240\346\240\207\346\214\211\344\270\213\357\274\232\346\233\264\346\267\261\344\270\200\347\202\271\347\232\204\347\201\260\350\211\262\357\274\210#DCDCDC\357\274\214\350\264\250\346\204\237\346\262\211\347\250\263\357\274\211 */\n"
"#minimize_botton:pressed {\n"
"    background-color: #CCCCCC !important;\n"
"}"));

        horizontalLayout_9->addWidget(minimize_botton);

        maximize_botton = new QPushButton(widget3_1_1);
        maximize_botton->setObjectName("maximize_botton");
        maximize_botton->setEnabled(true);
        maximize_botton->setMinimumSize(QSize(0, 30));
        maximize_botton->setMaximumSize(QSize(16777215, 30));
        QFont font2;
        font2.setFamilies({QString::fromUtf8("\351\273\221\344\275\223")});
        font2.setPointSize(10);
        font2.setBold(false);
        maximize_botton->setFont(font2);
        maximize_botton->setStyleSheet(QString::fromUtf8("#maximize_botton {\n"
"    /* \345\237\272\347\241\200\346\240\267\345\274\217\357\274\232\346\227\240\350\276\271\346\241\206 + \345\233\233\350\247\222\345\234\206\350\247\222 + \351\200\217\346\230\216\350\203\214\346\231\257 */\n"
"    border: none !important;                /* \345\275\273\345\272\225\351\232\220\350\227\217\350\276\271\346\241\206 */\n"
"\n"
"    background: transparent !important;     /* \345\210\235\345\247\213\351\200\217\346\230\216 */\n"
"\n"
"    outline: none !important;               /* \345\216\273\346\216\211\347\204\246\347\202\271\346\241\206\357\274\210QT\351\273\230\350\256\244\347\202\271\345\207\273\345\220\216\344\274\232\346\234\211\357\274\211 */\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\202\254\346\265\256\357\274\232\346\257\224#F0F0F0\347\250\215\346\267\261\347\232\204\347\201\260\350\211\262\357\274\210#EBEBEB\357\274\214\350\277\207\346\270\241\350\207\252\347\204\266\357\274\211 */\n"
"#maximize_botton:hover {\n"
"    background-color: #DCDCDC !important;\n"
""
                        "}\n"
"\n"
"/* \351\274\240\346\240\207\346\214\211\344\270\213\357\274\232\346\233\264\346\267\261\344\270\200\347\202\271\347\232\204\347\201\260\350\211\262\357\274\210#DCDCDC\357\274\214\350\264\250\346\204\237\346\262\211\347\250\263\357\274\211 */\n"
"#maximize_botton:pressed {\n"
"    background-color: #CCCCCC !important;\n"
"}"));

        horizontalLayout_9->addWidget(maximize_botton);

        close_botton = new QPushButton(widget3_1_1);
        close_botton->setObjectName("close_botton");
        close_botton->setEnabled(true);
        close_botton->setMinimumSize(QSize(0, 30));
        close_botton->setMaximumSize(QSize(16777215, 30));
        QFont font3;
        font3.setFamilies({QString::fromUtf8("\346\245\267\344\275\223")});
        font3.setPointSize(12);
        font3.setBold(false);
        close_botton->setFont(font3);
        close_botton->setStyleSheet(QString::fromUtf8("#close_botton {\n"
"    /* \345\237\272\347\241\200\346\240\267\345\274\217\357\274\232\346\227\240\350\276\271\346\241\206 + \345\233\233\350\247\222\345\234\206\350\247\222 + \351\200\217\346\230\216\350\203\214\346\231\257 */\n"
"    border: none !important;                /* \345\275\273\345\272\225\351\232\220\350\227\217\350\276\271\346\241\206 */\n"
"\n"
"    background: transparent !important;     /* \345\210\235\345\247\213\351\200\217\346\230\216 */\n"
"    /* \345\217\263\344\270\212\350\247\222\345\234\206\350\247\2228px\357\274\210\344\277\235\346\214\201\345\216\237\345\247\213\345\200\274\357\274\214\344\270\215\303\2275\357\274\211 */\n"
"    border-top-right-radius: 8px !important;\n"
"\n"
"    outline: none !important;               /* \345\216\273\346\216\211\347\204\246\347\202\271\346\241\206\357\274\210QT\351\273\230\350\256\244\347\202\271\345\207\273\345\220\216\344\274\232\346\234\211\357\274\211 */\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\202\254\346\265\256\357\274\232\346\257\224"
                        "#F0F0F0\347\250\215\346\267\261\347\232\204\347\201\260\350\211\262\357\274\210#EBEBEB\357\274\214\350\277\207\346\270\241\350\207\252\347\204\266\357\274\211 */\n"
"#close_botton:hover {\n"
"    background-color: #E52222 !important;\n"
"    color: #FFFFFF !important; \n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\214\211\344\270\213\357\274\232\346\233\264\346\267\261\344\270\200\347\202\271\347\232\204\347\201\260\350\211\262\357\274\210#DCDCDC\357\274\214\350\264\250\346\204\237\346\262\211\347\250\263\357\274\211 */\n"
"#close_botton:pressed {\n"
"    background-color: #D01E1E !important;\n"
"    color: #FFFFFF !important; \n"
"}"));

        horizontalLayout_9->addWidget(close_botton);


        horizontalLayout->addWidget(widget3_1_1);


        verticalLayout_5->addWidget(widget3_1);

        widget3_2 = new QWidget(stackedWidget3_chat);
        widget3_2->setObjectName("widget3_2");
        widget3_2->setMinimumSize(QSize(0, 50));
        widget3_2->setMaximumSize(QSize(16777215, 50));
        horizontalLayout_6 = new QHBoxLayout(widget3_2);
        horizontalLayout_6->setSpacing(0);
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        horizontalLayout_6->setContentsMargins(0, 0, 0, 0);
        widget3_2_2 = new QWidget(widget3_2);
        widget3_2_2->setObjectName("widget3_2_2");
        widget3_2_2->setMinimumSize(QSize(198, 0));
        widget3_2_2->setMaximumSize(QSize(200, 16777215));
        horizontalLayout_7 = new QHBoxLayout(widget3_2_2);
        horizontalLayout_7->setSpacing(0);
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        horizontalLayout_7->setContentsMargins(20, 0, 0, 10);
        person_name = new QLabel(widget3_2_2);
        person_name->setObjectName("person_name");
        QFont font4;
        font4.setPointSize(12);
        person_name->setFont(font4);

        horizontalLayout_7->addWidget(person_name);


        horizontalLayout_6->addWidget(widget3_2_2);

        horizontalSpacer3_2 = new QSpacerItem(455, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer3_2);

        widget3_2_1 = new QWidget(widget3_2);
        widget3_2_1->setObjectName("widget3_2_1");
        widget3_2_1->setMinimumSize(QSize(80, 50));
        widget3_2_1->setMaximumSize(QSize(80, 50));

        horizontalLayout_6->addWidget(widget3_2_1);


        verticalLayout_5->addWidget(widget3_2);

        widget3_3 = new QWidget(stackedWidget3_chat);
        widget3_3->setObjectName("widget3_3");
        QFont font5;
        font5.setFamilies({QString::fromUtf8("\346\226\271\346\255\243\347\262\227\351\273\221\345\256\213\347\256\200\344\275\223")});
        font5.setPointSize(12);
        widget3_3->setFont(font5);
        widget3_3->setStyleSheet(QString::fromUtf8("#widget3_3 {\n"
"    /* \346\265\205\347\201\260\350\211\262\350\203\214\346\231\257\357\274\210\344\277\235\347\225\231\345\216\237\346\234\211\357\274\211 */\n"
"    background-color: #F0F0F0 !important;\n"
"    /* \344\273\205\346\230\276\347\244\272\344\270\212\344\270\213\350\276\271\346\241\206\357\274\214\345\267\246\345\217\263\346\227\240\350\276\271\346\241\206\357\274\210\346\240\270\345\277\203\344\277\256\346\224\271\357\274\211 */\n"
"    border-top: 0.5px solid #D6D6D6 !important;    /* \344\270\212\350\276\271\346\241\206\357\274\2320.5px\346\267\261\347\201\260\345\256\236\345\277\203 */\n"
"    border-bottom: 0.5px solid #D6D6D6 !important; /* \344\270\213\350\276\271\346\241\206\357\274\2320.5px\346\267\261\347\201\260\345\256\236\345\277\203 */\n"
"    border-left: none !important;                /* \345\267\246\350\276\271\346\241\206\357\274\232\351\232\220\350\227\217 */\n"
"    border-right: none !important;               /* \345\217\263\350\276\271\346\241\206\357\274\232\351\232\220\350"
                        "\227\217 */\n"
"    /* \345\274\272\345\210\266\346\230\276\347\244\272\357\274\214\346\216\222\351\231\244\351\232\220\350\227\217/\351\200\217\346\230\216\345\271\262\346\211\260\357\274\210\344\277\235\347\225\231\345\216\237\346\234\211\357\274\211 */\n"
"    visibility: visible !important;\n"
"    opacity: 1 !important;\n"
"}"));
        horizontalLayout_8 = new QHBoxLayout(widget3_3);
        horizontalLayout_8->setSpacing(0);
        horizontalLayout_8->setObjectName("horizontalLayout_8");
        horizontalLayout_8->setContentsMargins(0, 0, 0, 0);
        chat_info_show = new QListView(widget3_3);
        chat_info_show->setObjectName("chat_info_show");
        chat_info_show->setStyleSheet(QString::fromUtf8(""));

        horizontalLayout_8->addWidget(chat_info_show);


        verticalLayout_5->addWidget(widget3_3);

        widget3_4 = new QWidget(stackedWidget3_chat);
        widget3_4->setObjectName("widget3_4");
        widget3_4->setMinimumSize(QSize(0, 30));
        widget3_4->setMaximumSize(QSize(16777215, 30));
        horizontalLayout_4 = new QHBoxLayout(widget3_4);
        horizontalLayout_4->setSpacing(0);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer3_4 = new QSpacerItem(385, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer3_4);

        widget3_1_2 = new QWidget(widget3_4);
        widget3_1_2->setObjectName("widget3_1_2");
        sizePolicy.setHeightForWidth(widget3_1_2->sizePolicy().hasHeightForWidth());
        widget3_1_2->setSizePolicy(sizePolicy);
        widget3_1_2->setMinimumSize(QSize(130, 30));
        widget3_1_2->setMaximumSize(QSize(150, 30));
        filetran = new QPushButton(widget3_1_2);
        filetran->setObjectName("filetran");
        filetran->setEnabled(true);
        filetran->setGeometry(QRect(40, 0, 30, 30));
        filetran->setMinimumSize(QSize(30, 30));
        filetran->setMaximumSize(QSize(30, 30));
        QFont font6;
        font6.setFamilies({QString::fromUtf8("\345\256\213\344\275\223")});
        font6.setPointSize(12);
        filetran->setFont(font6);
        filetran->setStyleSheet(QString::fromUtf8("QPushButton#filetran {\n"
"    border-image: url(btnimage/\346\226\207\344\273\266\344\270\212\344\274\240.png) 0 0 0 0 stretch stretch;\n"
"    border: none;\n"
"    background-color: transparent;\n"
"    padding: 0px !important;\n"
"}"));
        getimage = new QPushButton(widget3_1_2);
        getimage->setObjectName("getimage");
        getimage->setEnabled(true);
        getimage->setGeometry(QRect(70, 0, 30, 30));
        getimage->setMinimumSize(QSize(30, 30));
        getimage->setMaximumSize(QSize(30, 30));
        getimage->setFont(font6);
        getimage->setStyleSheet(QString::fromUtf8("QPushButton#getimage {\n"
"    border-image: url(btnimage/\346\210\252\345\233\276.png) 0 0 0 0 stretch stretch;\n"
"    border: none;\n"
"    background-color: transparent;\n"
"    padding: 0px !important;\n"
"}"));
        chatrecord = new QPushButton(widget3_1_2);
        chatrecord->setObjectName("chatrecord");
        chatrecord->setEnabled(true);
        chatrecord->setGeometry(QRect(100, 0, 30, 30));
        chatrecord->setMinimumSize(QSize(30, 30));
        chatrecord->setMaximumSize(QSize(30, 30));
        QFont font7;
        font7.setFamilies({QString::fromUtf8("\345\256\213\344\275\223")});
        font7.setPointSize(12);
        font7.setBold(false);
        chatrecord->setFont(font7);
        chatrecord->setStyleSheet(QString::fromUtf8("QPushButton#chatrecord {\n"
"    border-image: url(btnimage/\350\201\212\345\244\251\350\256\260\345\275\225.png) 0 0 0 0 stretch stretch;\n"
"    border: none;\n"
"    background-color: transparent;\n"
"    padding: 0px !important;\n"
"}"));

        horizontalLayout_4->addWidget(widget3_1_2);


        verticalLayout_5->addWidget(widget3_4);

        widget3_5 = new QWidget(stackedWidget3_chat);
        widget3_5->setObjectName("widget3_5");
        widget3_5->setMinimumSize(QSize(0, 120));
        widget3_5->setMaximumSize(QSize(16777215, 120));
        horizontalLayout_3 = new QHBoxLayout(widget3_5);
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        chat_input = new ChatInputTextEdit(widget3_5);
        chat_input->setObjectName("chat_input");
        chat_input->setStyleSheet(QString::fromUtf8("#chat_input {\n"
"    /* \342\221\240 \345\275\273\345\272\225\351\232\220\350\227\217\350\276\271\346\241\206\357\274\210\345\214\205\346\213\254\351\273\230\350\256\244\347\232\204\347\204\246\347\202\271\350\276\271\346\241\206\357\274\211 */\n"
"    border: none !important;\n"
"    outline: none !important;\n"
"    \n"
"    \n"
"    /* \342\221\242 \345\272\225\350\211\262\350\256\276\344\270\272#F0F0F0\347\201\260\350\211\262 */\n"
"    background-color: #F0F0F0 !important;\n"
"    \n"
"    /* \344\274\230\345\214\226\357\274\232\345\206\205\350\276\271\350\267\235\357\274\214\351\201\277\345\205\215\346\226\207\345\255\227\350\264\264\345\234\206\350\247\222\350\276\271\347\274\230\357\274\210\345\217\257\351\200\211\344\275\206\346\216\250\350\215\220\357\274\211 */\n"
"    padding: 6px 12px !important;\n"
"}"));
        chat_input->setReadOnly(false);

        horizontalLayout_3->addWidget(chat_input);


        verticalLayout_5->addWidget(widget3_5);

        widget3_6 = new QWidget(stackedWidget3_chat);
        widget3_6->setObjectName("widget3_6");
        widget3_6->setMinimumSize(QSize(0, 50));
        widget3_6->setMaximumSize(QSize(16777215, 50));
        widget3_6->setStyleSheet(QString::fromUtf8("#widget3_6 {\n"
"    /* \342\221\240 \345\275\273\345\272\225\351\232\220\350\227\217\350\276\271\346\241\206\357\274\210\345\214\205\346\213\254\351\273\230\350\256\244\347\232\204\347\204\246\347\202\271\350\276\271\346\241\206\357\274\211 */\n"
"    border: none !important;\n"
"    outline: none !important;\n"
"    \n"
"    \n"
"    /* \342\221\242 \345\272\225\350\211\262\350\256\276\344\270\272#F0F0F0\347\201\260\350\211\262 */\n"
"    background-color: #F0F0F0 !important;\n"
"    \n"
"    /* \344\274\230\345\214\226\357\274\232\345\206\205\350\276\271\350\267\235\357\274\214\351\201\277\345\205\215\346\226\207\345\255\227\350\264\264\345\234\206\350\247\222\350\276\271\347\274\230\357\274\210\345\217\257\351\200\211\344\275\206\346\216\250\350\215\220\357\274\211 */\n"
"    padding: 6px 12px !important;\n"
"}"));
        horizontalLayout_5 = new QHBoxLayout(widget3_6);
        horizontalLayout_5->setSpacing(0);
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        widgetfile = new QWidget(widget3_6);
        widgetfile->setObjectName("widgetfile");
        widgetfile->setMinimumSize(QSize(250, 0));
        widgetfile->setMaximumSize(QSize(250, 16777215));
        horizontalLayout_12 = new QHBoxLayout(widgetfile);
        horizontalLayout_12->setSpacing(0);
        horizontalLayout_12->setObjectName("horizontalLayout_12");
        horizontalLayout_12->setContentsMargins(20, 0, 0, 0);
        fileprogress = new QProgressBar(widgetfile);
        fileprogress->setObjectName("fileprogress");
        fileprogress->setStyleSheet(QString::fromUtf8("/* \346\255\243\347\241\256\357\274\232#fileprogress \345\214\271\351\205\215objectName\344\270\272fileprogress\347\232\204\346\216\247\344\273\266\357\274\210\346\225\264\344\275\223\346\240\267\345\274\217\357\274\211 */\n"
"#fileprogress {\n"
"    border: 1px solid #E0E0E0; /* \350\277\233\345\272\246\346\235\241\350\276\271\346\241\206\357\274\210\347\201\260\350\211\262\357\274\214\345\217\257\350\207\252\345\256\232\344\271\211\357\274\211 */\n"
"    border-radius: 8px; /* \350\277\233\345\272\246\346\235\241\345\234\206\350\247\222\357\274\210\345\217\257\351\200\211\357\274\214\346\233\264\347\276\216\350\247\202\357\274\211 */\n"
"    background-color: #F5F5F5; /* \350\277\233\345\272\246\346\235\241\346\234\252\345\241\253\345\205\205\350\203\214\346\231\257\350\211\262\357\274\210\346\265\205\347\201\260\357\274\211 */\n"
"    text-align: center; /* \350\277\233\345\272\246\346\226\207\345\255\227\345\261\205\344\270\255\357\274\210\345\217\257\351\200\211\357\274\211 */\n"
"}\n"
"\n"
"/* \346\255\243"
                        "\347\241\256\357\274\232#fileprogress::chunk \345\214\271\351\205\215\350\257\245\346\216\247\344\273\266\347\232\204\345\267\262\345\241\253\345\205\205\350\277\233\345\272\246\345\235\227\357\274\210\346\240\270\345\277\203\351\242\234\350\211\262\357\274\211 */\n"
"#fileprogress::chunk {\n"
"    background-color: rgb(154, 242, 109); /* \347\233\256\346\240\207RGB\351\242\234\350\211\262\357\274\214\345\256\214\345\205\250\346\224\257\346\214\201 */\n"
"    border-radius: 4px; /* \350\277\233\345\272\246\345\235\227\345\234\206\350\247\222\357\274\210\345\222\214\346\225\264\344\275\223\350\277\233\345\272\246\346\235\241\345\257\271\345\272\224\357\274\214\345\217\257\351\200\211\357\274\211 */\n"
"}"));
        fileprogress->setValue(24);
        fileprogress->setTextVisible(true);

        horizontalLayout_12->addWidget(fileprogress);


        horizontalLayout_5->addWidget(widgetfile);

        horizontalSpacer3_6 = new QSpacerItem(415, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer3_6);

        widget3_6_1 = new QWidget(widget3_6);
        widget3_6_1->setObjectName("widget3_6_1");
        widget3_6_1->setMinimumSize(QSize(120, 0));
        widget3_6_1->setMaximumSize(QSize(120, 16777215));
        send_botton = new QPushButton(widget3_6_1);
        send_botton->setObjectName("send_botton");
        send_botton->setEnabled(true);
        send_botton->setGeometry(QRect(10, 9, 100, 31));
        QFont font8;
        font8.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font8.setPointSize(10);
        send_botton->setFont(font8);
        send_botton->setStyleSheet(QString::fromUtf8("#send_botton {\n"
"    /* \345\237\272\347\241\200\346\240\267\345\274\217\357\274\232\345\276\256\344\277\241\345\216\237\347\224\237\345\217\221\351\200\201\346\214\211\351\222\256\346\267\261\347\273\277 + \344\277\235\347\225\231\345\216\237\346\234\211\351\205\215\347\275\256 */\n"
"    border: none !important;                /* \345\275\273\345\272\225\351\232\220\350\227\217\350\276\271\346\241\206 */\n"
"    border-radius: 4px !important;          /* \345\233\233\350\247\222\345\234\206\350\247\222\357\274\210\345\214\271\351\205\215\345\276\256\344\277\241\346\214\211\351\222\256\345\234\206\350\247\222\357\274\211 */\n"
"    background-color: #07C160 !important;   /* \345\270\270\346\200\201\357\274\232\345\276\256\344\277\241\347\273\217\345\205\270\345\217\221\351\200\201\346\214\211\351\222\256\347\273\277\357\274\210\346\240\270\345\277\203\350\211\262\357\274\211 */\n"
"    outline: none !important;               /* \345\216\273\346\216\211\347\204\246\347\202\271\346\241\206 */\n"
"    color: #"
                        "FFFFFF !important;              /* \345\255\227\344\275\223\347\272\257\347\231\275\350\211\262\357\274\210\345\222\214\345\276\256\344\277\241\344\270\200\350\207\264\357\274\211 */\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\202\254\346\265\256\357\274\232\346\257\224\345\276\256\344\277\241\345\270\270\346\200\201\347\273\277\347\250\215\346\267\261\357\274\214\350\247\206\350\247\211\345\217\215\351\246\210\350\207\252\347\204\266 */\n"
"#send_botton:hover {\n"
"    background-color: #06B058 !important;   /* \346\202\254\346\265\256\357\274\232\346\267\261\344\270\200\347\272\247\345\276\256\344\277\241\347\273\277 */\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\214\211\344\270\213\357\274\232\346\233\264\346\267\261\347\232\204\347\273\277\350\211\262\357\274\214\346\250\241\346\213\237\346\214\211\345\216\213\344\270\213\346\262\211\347\232\204\350\264\250\346\204\237 */\n"
"#send_botton:pressed {\n"
"    background-color: #059E4E !important;   /* \346\214\211\344\270\213\357\274\232\346\234\200\346"
                        "\267\261\347\272\247\345\276\256\344\277\241\347\273\277 */\n"
"}"));

        horizontalLayout_5->addWidget(widget3_6_1);


        verticalLayout_5->addWidget(widget3_6);

        stackedWidget3->addWidget(stackedWidget3_chat);
        NewFriendInfoWidget = new QWidget();
        NewFriendInfoWidget->setObjectName("NewFriendInfoWidget");
        verticalLayout_6 = new QVBoxLayout(NewFriendInfoWidget);
        verticalLayout_6->setSpacing(0);
        verticalLayout_6->setObjectName("verticalLayout_6");
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        widget3_7 = new QWidget(NewFriendInfoWidget);
        widget3_7->setObjectName("widget3_7");
        widget3_7->setMinimumSize(QSize(0, 30));
        widget3_7->setMaximumSize(QSize(16777215, 30));
        horizontalLayout_10 = new QHBoxLayout(widget3_7);
        horizontalLayout_10->setSpacing(0);
        horizontalLayout_10->setObjectName("horizontalLayout_10");
        horizontalLayout_10->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_2 = new QSpacerItem(385, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_2);

        widget3_1_3 = new QWidget(widget3_7);
        widget3_1_3->setObjectName("widget3_1_3");
        sizePolicy.setHeightForWidth(widget3_1_3->sizePolicy().hasHeightForWidth());
        widget3_1_3->setSizePolicy(sizePolicy);
        widget3_1_3->setMinimumSize(QSize(120, 30));
        widget3_1_3->setMaximumSize(QSize(120, 30));
        horizontalLayout_11 = new QHBoxLayout(widget3_1_3);
        horizontalLayout_11->setSpacing(0);
        horizontalLayout_11->setObjectName("horizontalLayout_11");
        horizontalLayout_11->setContentsMargins(0, 0, 0, 0);
        minimize_botton_2 = new QPushButton(widget3_1_3);
        minimize_botton_2->setObjectName("minimize_botton_2");
        minimize_botton_2->setEnabled(true);
        minimize_botton_2->setMinimumSize(QSize(0, 30));
        minimize_botton_2->setMaximumSize(QSize(16777215, 30));
        minimize_botton_2->setFont(font1);
        minimize_botton_2->setStyleSheet(QString::fromUtf8("#minimize_botton_2 {\n"
"    /* \345\237\272\347\241\200\346\240\267\345\274\217\357\274\232\346\227\240\350\276\271\346\241\206 + \345\233\233\350\247\222\345\234\206\350\247\222 + \351\200\217\346\230\216\350\203\214\346\231\257 */\n"
"    border: none !important;                /* \345\275\273\345\272\225\351\232\220\350\227\217\350\276\271\346\241\206 */\n"
"\n"
"    background: transparent !important;     /* \345\210\235\345\247\213\351\200\217\346\230\216 */\n"
"\n"
"    outline: none !important;               /* \345\216\273\346\216\211\347\204\246\347\202\271\346\241\206\357\274\210QT\351\273\230\350\256\244\347\202\271\345\207\273\345\220\216\344\274\232\346\234\211\357\274\211 */\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\202\254\346\265\256\357\274\232\346\257\224#F0F0F0\347\250\215\346\267\261\347\232\204\347\201\260\350\211\262\357\274\210#EBEBEB\357\274\214\350\277\207\346\270\241\350\207\252\347\204\266\357\274\211 */\n"
"#minimize_botton_2:hover {\n"
"    background-color: #DCDCDC !importan"
                        "t;\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\214\211\344\270\213\357\274\232\346\233\264\346\267\261\344\270\200\347\202\271\347\232\204\347\201\260\350\211\262\357\274\210#DCDCDC\357\274\214\350\264\250\346\204\237\346\262\211\347\250\263\357\274\211 */\n"
"#minimize_botton_2:pressed {\n"
"    background-color: #CCCCCC !important;\n"
"}"));

        horizontalLayout_11->addWidget(minimize_botton_2);

        maximize_botton_2 = new QPushButton(widget3_1_3);
        maximize_botton_2->setObjectName("maximize_botton_2");
        maximize_botton_2->setEnabled(true);
        maximize_botton_2->setMinimumSize(QSize(0, 30));
        maximize_botton_2->setMaximumSize(QSize(16777215, 30));
        maximize_botton_2->setFont(font2);
        maximize_botton_2->setStyleSheet(QString::fromUtf8("#maximize_botton_2 {\n"
"    /* \345\237\272\347\241\200\346\240\267\345\274\217\357\274\232\346\227\240\350\276\271\346\241\206 + \345\233\233\350\247\222\345\234\206\350\247\222 + \351\200\217\346\230\216\350\203\214\346\231\257 */\n"
"    border: none !important;                /* \345\275\273\345\272\225\351\232\220\350\227\217\350\276\271\346\241\206 */\n"
"\n"
"    background: transparent !important;     /* \345\210\235\345\247\213\351\200\217\346\230\216 */\n"
"\n"
"    outline: none !important;               /* \345\216\273\346\216\211\347\204\246\347\202\271\346\241\206\357\274\210QT\351\273\230\350\256\244\347\202\271\345\207\273\345\220\216\344\274\232\346\234\211\357\274\211 */\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\202\254\346\265\256\357\274\232\346\257\224#F0F0F0\347\250\215\346\267\261\347\232\204\347\201\260\350\211\262\357\274\210#EBEBEB\357\274\214\350\277\207\346\270\241\350\207\252\347\204\266\357\274\211 */\n"
"#maximize_botton_2:hover {\n"
"    background-color: #DCDCDC !importan"
                        "t;\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\214\211\344\270\213\357\274\232\346\233\264\346\267\261\344\270\200\347\202\271\347\232\204\347\201\260\350\211\262\357\274\210#DCDCDC\357\274\214\350\264\250\346\204\237\346\262\211\347\250\263\357\274\211 */\n"
"#maximize_botton_2:pressed {\n"
"    background-color: #CCCCCC !important;\n"
"}"));

        horizontalLayout_11->addWidget(maximize_botton_2);

        close_botton_2 = new QPushButton(widget3_1_3);
        close_botton_2->setObjectName("close_botton_2");
        close_botton_2->setEnabled(true);
        close_botton_2->setMinimumSize(QSize(0, 30));
        close_botton_2->setMaximumSize(QSize(16777215, 30));
        close_botton_2->setFont(font3);
        close_botton_2->setStyleSheet(QString::fromUtf8("#close_botton_2 {\n"
"    /* \345\237\272\347\241\200\346\240\267\345\274\217\357\274\232\346\227\240\350\276\271\346\241\206 + \345\233\233\350\247\222\345\234\206\350\247\222 + \351\200\217\346\230\216\350\203\214\346\231\257 */\n"
"    border: none !important;                /* \345\275\273\345\272\225\351\232\220\350\227\217\350\276\271\346\241\206 */\n"
"\n"
"    background: transparent !important;     /* \345\210\235\345\247\213\351\200\217\346\230\216 */\n"
"    /* \345\217\263\344\270\212\350\247\222\345\234\206\350\247\2228px\357\274\210\344\277\235\346\214\201\345\216\237\345\247\213\345\200\274\357\274\214\344\270\215\303\2275\357\274\211 */\n"
"    border-top-right-radius: 8px !important;\n"
"\n"
"    outline: none !important;               /* \345\216\273\346\216\211\347\204\246\347\202\271\346\241\206\357\274\210QT\351\273\230\350\256\244\347\202\271\345\207\273\345\220\216\344\274\232\346\234\211\357\274\211 */\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\202\254\346\265\256\357\274\232\346\257"
                        "\224#F0F0F0\347\250\215\346\267\261\347\232\204\347\201\260\350\211\262\357\274\210#EBEBEB\357\274\214\350\277\207\346\270\241\350\207\252\347\204\266\357\274\211 */\n"
"#close_botton_2:hover {\n"
"    background-color: #E52222 !important;\n"
"    color: #FFFFFF !important; \n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\214\211\344\270\213\357\274\232\346\233\264\346\267\261\344\270\200\347\202\271\347\232\204\347\201\260\350\211\262\357\274\210#DCDCDC\357\274\214\350\264\250\346\204\237\346\262\211\347\250\263\357\274\211 */\n"
"#close_botton_2:pressed {\n"
"    background-color: #D01E1E !important;\n"
"    color: #FFFFFF !important; \n"
"}"));

        horizontalLayout_11->addWidget(close_botton_2);


        horizontalLayout_10->addWidget(widget3_1_3);


        verticalLayout_6->addWidget(widget3_7);

        FriendInfoShowwidget = new QWidget(NewFriendInfoWidget);
        FriendInfoShowwidget->setObjectName("FriendInfoShowwidget");
        photoArea = new QLabel(FriendInfoShowwidget);
        photoArea->setObjectName("photoArea");
        photoArea->setGeometry(QRect(130, 60, 80, 80));
        nameArea = new QLabel(FriendInfoShowwidget);
        nameArea->setObjectName("nameArea");
        nameArea->setGeometry(QRect(230, 60, 150, 30));
        btnAgree = new QPushButton(FriendInfoShowwidget);
        btnAgree->setObjectName("btnAgree");
        btnAgree->setGeometry(QRect(130, 240, 90, 30));
        btnAgree->setFont(font4);
        btnAgree->setStyleSheet(QString::fromUtf8("#btnAgree {\n"
"    /* \345\237\272\347\241\200\346\240\267\345\274\217\357\274\232\345\276\256\344\277\241\345\216\237\347\224\237\345\217\221\351\200\201\346\214\211\351\222\256\346\267\261\347\273\277 + \344\277\235\347\225\231\345\216\237\346\234\211\351\205\215\347\275\256 */\n"
"    border: none !important;                /* \345\275\273\345\272\225\351\232\220\350\227\217\350\276\271\346\241\206 */\n"
"    border-radius: 4px !important;          /* \345\233\233\350\247\222\345\234\206\350\247\222\357\274\210\345\214\271\351\205\215\345\276\256\344\277\241\346\214\211\351\222\256\345\234\206\350\247\222\357\274\211 */\n"
"    background-color: #07C160 !important;   /* \345\270\270\346\200\201\357\274\232\345\276\256\344\277\241\347\273\217\345\205\270\345\217\221\351\200\201\346\214\211\351\222\256\347\273\277\357\274\210\346\240\270\345\277\203\350\211\262\357\274\211 */\n"
"    outline: none !important;               /* \345\216\273\346\216\211\347\204\246\347\202\271\346\241\206 */\n"
"    color: #FFF"
                        "FFF !important;              /* \345\255\227\344\275\223\347\272\257\347\231\275\350\211\262\357\274\210\345\222\214\345\276\256\344\277\241\344\270\200\350\207\264\357\274\211 */\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\202\254\346\265\256\357\274\232\346\257\224\345\276\256\344\277\241\345\270\270\346\200\201\347\273\277\347\250\215\346\267\261\357\274\214\350\247\206\350\247\211\345\217\215\351\246\210\350\207\252\347\204\266 */\n"
"#btnAgree:hover {\n"
"    background-color: #06B058 !important;   /* \346\202\254\346\265\256\357\274\232\346\267\261\344\270\200\347\272\247\345\276\256\344\277\241\347\273\277 */\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\214\211\344\270\213\357\274\232\346\233\264\346\267\261\347\232\204\347\273\277\350\211\262\357\274\214\346\250\241\346\213\237\346\214\211\345\216\213\344\270\213\346\262\211\347\232\204\350\264\250\346\204\237 */\n"
"#btnAgree:pressed {\n"
"    background-color: #059E4E !important;   /* \346\214\211\344\270\213\357\274\232\346\234\200\346\267\261\347"
                        "\272\247\345\276\256\344\277\241\347\273\277 */\n"
"}"));
        btnRefuse = new QPushButton(FriendInfoShowwidget);
        btnRefuse->setObjectName("btnRefuse");
        btnRefuse->setGeometry(QRect(300, 240, 90, 30));
        btnRefuse->setFont(font4);
        btnRefuse->setStyleSheet(QString::fromUtf8("#btnRefuse {\n"
"    /* \345\237\272\347\241\200\346\240\267\345\274\217\357\274\232\345\276\256\344\277\241\345\216\237\347\224\237\345\217\221\351\200\201\346\214\211\351\222\256\346\267\261\347\273\277 + \344\277\235\347\225\231\345\216\237\346\234\211\351\205\215\347\275\256 */\n"
"    border: none !important;                /* \345\275\273\345\272\225\351\232\220\350\227\217\350\276\271\346\241\206 */\n"
"    border-radius: 4px !important;          /* \345\233\233\350\247\222\345\234\206\350\247\222\357\274\210\345\214\271\351\205\215\345\276\256\344\277\241\346\214\211\351\222\256\345\234\206\350\247\222\357\274\211 */\n"
"    background-color: #F0F0F0 !important;   /* \345\270\270\346\200\201\357\274\232\345\276\256\344\277\241\347\273\217\345\205\270\345\217\221\351\200\201\346\214\211\351\222\256\347\273\277\357\274\210\346\240\270\345\277\203\350\211\262\357\274\211 */\n"
"    outline: none !important;               /* \345\216\273\346\216\211\347\204\246\347\202\271\346\241\206 */\n"
"\n"
"}\n"
"\n"
""
                        "/* \351\274\240\346\240\207\346\202\254\346\265\256\357\274\232\346\257\224#F0F0F0\347\250\215\346\267\261\347\232\204\347\201\260\350\211\262\357\274\210#EBEBEB\357\274\214\350\277\207\346\270\241\350\207\252\347\204\266\357\274\211 */\n"
"#btnRefuse:hover {\n"
"    background-color: #DCDCDC !important;\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\214\211\344\270\213\357\274\232\346\233\264\346\267\261\344\270\200\347\202\271\347\232\204\347\201\260\350\211\262\357\274\210#DCDCDC\357\274\214\350\264\250\346\204\237\346\262\211\347\250\263\357\274\211 */\n"
"#btnRefuse:pressed {\n"
"    background-color: #CCCCCC !important;\n"
"}"));
        cheakinfoArea = new QLabel(FriendInfoShowwidget);
        cheakinfoArea->setObjectName("cheakinfoArea");
        cheakinfoArea->setGeometry(QRect(230, 110, 150, 30));
        Friend_status = new QLabel(FriendInfoShowwidget);
        Friend_status->setObjectName("Friend_status");
        Friend_status->setGeometry(QRect(190, 180, 150, 30));

        verticalLayout_6->addWidget(FriendInfoShowwidget);

        stackedWidget3->addWidget(NewFriendInfoWidget);

        horizontalLayout_2->addWidget(stackedWidget3);

        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        stackedWidget2->setCurrentIndex(0);
        stackedWidget3->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        userAvatar->setText(QString());
        btnLookFriend->setText(QString());
        btnStartChat->setText(QString());
        groupchat->setText(QString());
        btnAddFriends->setText(QCoreApplication::translate("MainWindow", "+", nullptr));
        btnCreateGroup->setText(QCoreApplication::translate("MainWindow", "+", nullptr));
        minimize_botton->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        maximize_botton->setText(QCoreApplication::translate("MainWindow", "\342\226\241", nullptr));
        close_botton->setText(QCoreApplication::translate("MainWindow", "\303\227", nullptr));
        person_name->setText(QString());
        filetran->setText(QString());
        getimage->setText(QString());
        chatrecord->setText(QString());
        send_botton->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201(S)", nullptr));
        minimize_botton_2->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        maximize_botton_2->setText(QCoreApplication::translate("MainWindow", "\342\226\241", nullptr));
        close_botton_2->setText(QCoreApplication::translate("MainWindow", "\303\227", nullptr));
        photoArea->setText(QCoreApplication::translate("MainWindow", "\345\245\275\345\217\213\345\244\264\345\203\217", nullptr));
        nameArea->setText(QCoreApplication::translate("MainWindow", "\345\276\205\345\220\214\346\204\217\347\232\204\345\245\275\345\217\213\347\232\204\350\264\246\345\217\267", nullptr));
        btnAgree->setText(QCoreApplication::translate("MainWindow", "\345\220\214\346\204\217", nullptr));
        btnRefuse->setText(QCoreApplication::translate("MainWindow", "\346\213\222\347\273\235", nullptr));
        cheakinfoArea->setText(QCoreApplication::translate("MainWindow", "\351\252\214\350\257\201\346\266\210\346\201\257", nullptr));
        Friend_status->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
