/********************************************************************************
** Form generated from reading UI file 'AddFriends.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDFRIENDS_H
#define UI_ADDFRIENDS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AddFriends
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *shadowwidget;
    QVBoxLayout *verticalLayout_2;
    QWidget *widget_1;
    QHBoxLayout *horizontalLayout_2;
    QWidget *widget_1_2;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_1_2;
    QSpacerItem *horizontalSpacer;
    QWidget *widget_1_1;
    QHBoxLayout *horizontalLayout;
    QPushButton *close_botton;
    QWidget *widget_2;
    QLineEdit *WriteFriendNumber;
    QLabel *NewFriendPhoto;
    QPushButton *SearchFriend;
    QLabel *NewFriendNumber;
    QLineEdit *CheckInfo;
    QLabel *FriendStatus;
    QWidget *widget_3;
    QPushButton *AddthisFriend;

    void setupUi(QWidget *AddFriends)
    {
        if (AddFriends->objectName().isEmpty())
            AddFriends->setObjectName("AddFriends");
        AddFriends->resize(400, 320);
        AddFriends->setStyleSheet(QString::fromUtf8(""));
        verticalLayout = new QVBoxLayout(AddFriends);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(5, 5, 5, 5);
        shadowwidget = new QWidget(AddFriends);
        shadowwidget->setObjectName("shadowwidget");
        verticalLayout_2 = new QVBoxLayout(shadowwidget);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        widget_1 = new QWidget(shadowwidget);
        widget_1->setObjectName("widget_1");
        widget_1->setMinimumSize(QSize(0, 35));
        widget_1->setMaximumSize(QSize(16777215, 35));
        widget_1->setStyleSheet(QString::fromUtf8("#widget_1 {\n"
"    /* \346\265\205\347\201\260\350\211\262\350\203\214\346\231\257\357\274\210\345\276\256\344\277\241PC\347\253\257\345\270\270\347\224\250\346\265\205\347\201\260\357\274\211 */\n"
"    background-color: #FFFFFF !important;\n"
"\n"
"    /* \345\267\246\344\270\212\350\247\222\345\234\206\350\247\2228px\357\274\210\344\277\235\346\214\201\345\216\237\345\247\213\345\200\274\357\274\214\344\270\215\303\2275\357\274\211 */\n"
"    border-top-left-radius: 8px !important;\n"
"    /* \345\267\246\344\270\213\350\247\222\345\234\206\350\247\2228px\357\274\210\344\277\235\346\214\201\345\216\237\345\247\213\345\200\274\357\274\211 */\n"
"    /* border-bottom-left-radius: 8px !important;*/\n"
"    border-top-right-radius: 8px !important;\n"
"    /* \345\217\263\344\270\212/\345\217\263\344\270\213\344\270\215\350\256\276\345\234\206\350\247\222\357\274\214\351\273\230\350\256\244\345\260\226\350\247\222\357\274\214\346\227\240\351\234\200\351\242\235\345\244\226\345\206\231 */\n"
"    /* \345\274\272"
                        "\345\210\266\346\230\276\347\244\272\357\274\214\346\216\222\351\231\244\351\232\220\350\227\217/\351\200\217\346\230\216\345\271\262\346\211\260 */\n"
"    visibility: visible !important;\n"
"    opacity: 1 !important;\n"
"}\n"
""));
        horizontalLayout_2 = new QHBoxLayout(widget_1);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        widget_1_2 = new QWidget(widget_1);
        widget_1_2->setObjectName("widget_1_2");
        widget_1_2->setMinimumSize(QSize(150, 0));
        widget_1_2->setMaximumSize(QSize(150, 16777215));
        horizontalLayout_3 = new QHBoxLayout(widget_1_2);
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        label_1_2 = new QLabel(widget_1_2);
        label_1_2->setObjectName("label_1_2");
        QFont font;
        font.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font.setPointSize(12);
        label_1_2->setFont(font);
        label_1_2->setStyleSheet(QString::fromUtf8("#label_1_2 {\n"
"    /* \346\265\205\347\201\260\350\211\262\346\226\207\345\255\227\357\274\210#CCCCCC\346\230\257\351\200\232\347\224\250\346\265\205\347\201\260\357\274\214\350\247\206\350\247\211\350\210\222\351\200\202\357\274\211 */\n"
"    color: #CCCCCC !important;\n"
"    /* \345\217\257\351\200\211\357\274\232\351\230\262\346\255\242\347\210\266\346\240\267\345\274\217\350\246\206\347\233\226\357\274\214\345\205\234\345\272\225 */\n"
"    background-color: transparent !important;\n"
"}"));

        horizontalLayout_3->addWidget(label_1_2);


        horizontalLayout_2->addWidget(widget_1_2);

        horizontalSpacer = new QSpacerItem(337, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        widget_1_1 = new QWidget(widget_1);
        widget_1_1->setObjectName("widget_1_1");
        widget_1_1->setEnabled(true);
        widget_1_1->setMinimumSize(QSize(50, 35));
        widget_1_1->setMaximumSize(QSize(50, 35));
        horizontalLayout = new QHBoxLayout(widget_1_1);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        close_botton = new QPushButton(widget_1_1);
        close_botton->setObjectName("close_botton");
        close_botton->setMinimumSize(QSize(0, 35));
        close_botton->setMaximumSize(QSize(16777215, 35));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("\346\245\267\344\275\223")});
        font1.setPointSize(16);
        close_botton->setFont(font1);
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
"\n"
"/* \351\274\240\346\240\207\346\202\254\346\265\256\357\274\232\346"
                        "\265\205\344\270\200\345\217\267\347\232\204\347\216\253\347\221\260\347\272\242 */\n"
"#close_botton:hover {\n"
"    background-color: #E02041 !important; /* \346\202\254\346\265\256\346\265\205\347\216\253\347\221\260\347\272\242\357\274\214\346\257\224\351\273\230\350\256\244\350\211\262\346\267\241 */\n"
"	color: #FFFFFF !important; /* \346\202\254\346\265\256\346\227\266\345\255\227\344\275\223\347\231\275\350\211\262 */\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\214\211\344\270\213\357\274\232\351\242\234\350\211\262\345\217\230\346\267\261\357\274\210\346\267\261\344\270\200\345\217\267\347\232\204\347\216\253\347\221\260\347\272\242\357\274\211 */\n"
"#close_botton:pressed {\n"
"    background-color: #C01838 !important; /* \346\214\211\344\270\213\346\267\261\347\216\253\347\221\260\347\272\242\357\274\214\346\257\224\351\273\230\350\256\244\350\211\262\346\267\261 */\n"
"	color: #FFFFFF !important; /* \346\202\254\346\265\256\346\227\266\345\255\227\344\275\223\347\231\275\350\211\262 */\n"
"}"));

        horizontalLayout->addWidget(close_botton);


        horizontalLayout_2->addWidget(widget_1_1);


        verticalLayout_2->addWidget(widget_1);

        widget_2 = new QWidget(shadowwidget);
        widget_2->setObjectName("widget_2");
        widget_2->setMinimumSize(QSize(0, 199));
        widget_2->setMaximumSize(QSize(16777215, 200));
        widget_2->setStyleSheet(QString::fromUtf8("#widget_2 {\n"
"    background-color: #FFFFFF !important;\n"
"    visibility: visible !important;\n"
"    opacity: 1 !important;\n"
"    border: none !important;\n"
"\n"
"    /* QT\345\205\250\347\211\210\346\234\254\345\205\274\345\256\271\347\232\204\345\267\246\345\217\263\351\230\264\345\275\261\357\274\210\345\215\225\351\230\264\345\275\261\345\206\231\346\263\225\357\274\214\346\227\240\350\257\255\346\263\225\351\224\231\350\257\257\357\274\211 */\n"
"    box-shadow: 0 0 10px 2px rgba(0, 0, 0, 0.08) !important;\n"
"\n"
"    qproperty-frameShape: NoFrame;\n"
"    qproperty-frameShadow: Plain;\n"
"    background-clip: padding-box !important;\n"
"\n"
"}"));
        WriteFriendNumber = new QLineEdit(widget_2);
        WriteFriendNumber->setObjectName("WriteFriendNumber");
        WriteFriendNumber->setGeometry(QRect(40, 20, 241, 40));
        WriteFriendNumber->setFont(font);
        WriteFriendNumber->setStyleSheet(QString::fromUtf8("#WriteFriendNumber {\n"
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
        WriteFriendNumber->setFrame(true);
        NewFriendPhoto = new QLabel(widget_2);
        NewFriendPhoto->setObjectName("NewFriendPhoto");
        NewFriendPhoto->setGeometry(QRect(50, 80, 80, 80));
        QFont font2;
        font2.setFamilies({QString::fromUtf8("\345\215\216\345\205\211\347\262\227\351\273\221_CNKI")});
        font2.setPointSize(20);
        NewFriendPhoto->setFont(font2);
        SearchFriend = new QPushButton(widget_2);
        SearchFriend->setObjectName("SearchFriend");
        SearchFriend->setGeometry(QRect(290, 20, 80, 40));
        QFont font3;
        font3.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font3.setPointSize(12);
        font3.setBold(false);
        SearchFriend->setFont(font3);
        SearchFriend->setStyleSheet(QString::fromUtf8("#SearchFriend {\n"
"    /* \342\221\240 \345\275\273\345\272\225\351\232\220\350\227\217\350\276\271\346\241\206\357\274\210\345\214\205\346\213\254\351\273\230\350\256\244\347\232\204\347\204\246\347\202\271\350\276\271\346\241\206\357\274\211 */\n"
"    border: none !important;\n"
"    outline: none !important;\n"
"    \n"
"    /* \342\221\241 \345\234\206\350\247\222\347\237\251\345\275\242\357\274\21012px\345\276\256\344\277\241\350\276\223\345\205\245\346\241\206\345\270\270\347\224\250\345\234\206\350\247\222\357\274\214\344\277\235\347\225\231\345\216\237\346\234\211\350\256\276\350\256\241\357\274\211 */\n"
"    border-radius: 12px !important;\n"
"    \n"
"    /* \345\270\270\346\200\201\357\274\232\345\276\256\344\277\241\347\273\217\345\205\270\345\217\221\351\200\201\346\214\211\351\222\256\347\273\277\357\274\210\346\240\270\345\277\203\350\211\262 #07C160\357\274\211 */\n"
"    background-color: #07C160 !important;\n"
"    \n"
"    /* \345\255\227\344\275\223\351\242\234\350\211\262\350\256\276\344"
                        "\270\272\347\272\257\347\231\275\350\211\262\357\274\214\344\270\216\347\273\277\350\211\262\350\203\214\346\231\257\345\257\271\346\257\224\346\270\205\346\231\260 */\n"
"    color: #FFFFFF !important;\n"
"    \n"
"    /* \344\274\230\345\214\226\357\274\232\345\206\205\350\276\271\350\267\235\357\274\214\351\201\277\345\205\215\346\226\207\345\255\227\350\264\264\345\234\206\350\247\222\350\276\271\347\274\230\357\274\210\344\277\235\347\225\231\345\216\237\346\234\211\344\274\230\345\214\226\357\274\211 */\n"
"    padding: 6px 12px !important;\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\202\254\346\265\256\357\274\232\345\276\256\344\277\241\347\273\277\346\267\261\344\270\200\347\272\247\357\274\210#06B058\357\274\211\357\274\214\350\247\206\350\247\211\345\217\215\351\246\210\350\207\252\347\204\266 */\n"
"#SearchFriend:hover {\n"
"    background-color: #06B058 !important;\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\214\211\344\270\213\357\274\232\345\276\256\344\277\241\347\273\277\346\234\200\346\267"
                        "\261\347\272\247\357\274\210#059E4E\357\274\211\357\274\214\346\250\241\346\213\237\346\214\211\345\216\213\344\270\213\346\262\211\350\264\250\346\204\237 */\n"
"#SearchFriend:pressed {\n"
"    background-color: #059E4E !important;\n"
"}"));
        NewFriendNumber = new QLabel(widget_2);
        NewFriendNumber->setObjectName("NewFriendNumber");
        NewFriendNumber->setGeometry(QRect(150, 80, 180, 30));
        NewFriendNumber->setFont(font2);
        CheckInfo = new QLineEdit(widget_2);
        CheckInfo->setObjectName("CheckInfo");
        CheckInfo->setGeometry(QRect(150, 120, 201, 40));
        CheckInfo->setFont(font);
        CheckInfo->setStyleSheet(QString::fromUtf8("#CheckInfo {\n"
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
        CheckInfo->setFrame(true);
        FriendStatus = new QLabel(widget_2);
        FriendStatus->setObjectName("FriendStatus");
        FriendStatus->setGeometry(QRect(150, 120, 180, 30));
        FriendStatus->setFont(font);

        verticalLayout_2->addWidget(widget_2);

        widget_3 = new QWidget(shadowwidget);
        widget_3->setObjectName("widget_3");
        widget_3->setStyleSheet(QString::fromUtf8("#widget_3 {\n"
"    /* \346\265\205\347\201\260\350\211\262\350\203\214\346\231\257\357\274\210\345\276\256\344\277\241PC\347\253\257\345\270\270\347\224\250\346\265\205\347\201\260\357\274\211 */\n"
"    background-color: #FFFFFF !important;\n"
"\n"
"    /* \345\267\246\344\270\213\350\247\222\345\234\206\350\247\2228px\357\274\210\344\277\235\346\214\201\345\216\237\345\247\213\345\200\274\357\274\214\344\270\215\303\2275\357\274\211 */\n"
"    border-bottom-left-radius: 8px !important;\n"
"    /* \345\267\246\344\270\213\350\247\222\345\234\206\350\247\2228px\357\274\210\344\277\235\346\214\201\345\216\237\345\247\213\345\200\274\357\274\211 */\n"
"    /* border-bottom-left-radius: 8px !important;*/\n"
"    border-bottom-right-radius: 8px !important;\n"
"    /* \345\217\263\344\270\212/\345\217\263\344\270\213\344\270\215\350\256\276\345\234\206\350\247\222\357\274\214\351\273\230\350\256\244\345\260\226\350\247\222\357\274\214\346\227\240\351\234\200\351\242\235\345\244\226\345\206\231 */\n"
"    /* \345\274"
                        "\272\345\210\266\346\230\276\347\244\272\357\274\214\346\216\222\351\231\244\351\232\220\350\227\217/\351\200\217\346\230\216\345\271\262\346\211\260 */\n"
"    visibility: visible !important;\n"
"    opacity: 1 !important;\n"
"}"));
        AddthisFriend = new QPushButton(widget_3);
        AddthisFriend->setObjectName("AddthisFriend");
        AddthisFriend->setGeometry(QRect(89, 20, 201, 40));
        AddthisFriend->setFont(font3);
        AddthisFriend->setStyleSheet(QString::fromUtf8("#AddthisFriend {\n"
"    /* \345\237\272\347\241\200\346\240\267\345\274\217\357\274\232\345\276\256\344\277\241\345\216\237\347\224\237\345\217\221\351\200\201\346\214\211\351\222\256\346\267\261\347\273\277 + \344\277\235\347\225\231\345\216\237\346\234\211\351\205\215\347\275\256 */\n"
"    border: none !important;                /* \345\275\273\345\272\225\351\232\220\350\227\217\350\276\271\346\241\206 */\n"
"    border-radius: 4px !important;          /* \345\233\233\350\247\222\345\234\206\350\247\222\357\274\210\345\214\271\351\205\215\345\276\256\344\277\241\346\214\211\351\222\256\345\234\206\350\247\222\357\274\211 */\n"
"    background-color: #F0F0F0 !important;   /* \345\270\270\346\200\201\357\274\232\345\276\256\344\277\241\347\273\217\345\205\270\345\217\221\351\200\201\346\214\211\351\222\256\347\273\277\357\274\210\346\240\270\345\277\203\350\211\262\357\274\211 */\n"
"    outline: none !important;               /* \345\216\273\346\216\211\347\204\246\347\202\271\346\241\206 */\n"
"\n"
"}\n"
""
                        "\n"
"/* \351\274\240\346\240\207\346\202\254\346\265\256\357\274\232\346\257\224#F0F0F0\347\250\215\346\267\261\347\232\204\347\201\260\350\211\262\357\274\210#EBEBEB\357\274\214\350\277\207\346\270\241\350\207\252\347\204\266\357\274\211 */\n"
"#AddthisFriend:hover {\n"
"    background-color: #DCDCDC !important;\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\214\211\344\270\213\357\274\232\346\233\264\346\267\261\344\270\200\347\202\271\347\232\204\347\201\260\350\211\262\357\274\210#DCDCDC\357\274\214\350\264\250\346\204\237\346\262\211\347\250\263\357\274\211 */\n"
"#AddthisFriend:pressed {\n"
"    background-color: #CCCCCC !important;\n"
"}"));

        verticalLayout_2->addWidget(widget_3);


        verticalLayout->addWidget(shadowwidget);


        retranslateUi(AddFriends);

        QMetaObject::connectSlotsByName(AddFriends);
    } // setupUi

    void retranslateUi(QWidget *AddFriends)
    {
        AddFriends->setWindowTitle(QCoreApplication::translate("AddFriends", "Form", nullptr));
        label_1_2->setText(QCoreApplication::translate("AddFriends", "   singchat", nullptr));
        close_botton->setText(QCoreApplication::translate("AddFriends", "\303\227", nullptr));
        WriteFriendNumber->setText(QString());
        WriteFriendNumber->setPlaceholderText(QCoreApplication::translate("AddFriends", "\350\276\223\345\205\245\345\245\275\345\217\213\350\264\246\345\217\267\350\277\233\350\241\214\346\220\234\347\264\242", nullptr));
        NewFriendPhoto->setText(QCoreApplication::translate("AddFriends", "\345\244\264\345\203\217", nullptr));
        SearchFriend->setText(QCoreApplication::translate("AddFriends", "\346\220\234\347\264\242", nullptr));
        NewFriendNumber->setText(QCoreApplication::translate("AddFriends", "\345\245\275\345\217\213\350\264\246\345\217\267", nullptr));
        CheckInfo->setText(QString());
        CheckInfo->setPlaceholderText(QCoreApplication::translate("AddFriends", "\350\276\223\345\205\245\346\211\223\346\213\233\345\221\274\346\266\210\346\201\257", nullptr));
        FriendStatus->setText(QCoreApplication::translate("AddFriends", "\344\275\240\344\273\254\345\267\262\347\273\217\346\230\257\345\245\275\345\217\213", nullptr));
        AddthisFriend->setText(QCoreApplication::translate("AddFriends", "\346\267\273\345\212\240\345\210\260\351\200\232\350\256\257\345\275\225", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AddFriends: public Ui_AddFriends {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDFRIENDS_H
