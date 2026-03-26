/********************************************************************************
** Form generated from reading UI file 'CreateGroup.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CREATEGROUP_H
#define UI_CREATEGROUP_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CreateGroup
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
    QPushButton *groupclose;
    QWidget *widget_2;
    QLineEdit *groupname;
    QPushButton *btnAvatargroup;
    QWidget *widget_3;
    QVBoxLayout *verticalLayout_3;
    QWidget *group_widget;
    QVBoxLayout *verticalLayout_4;
    QListView *creategroupList;
    QWidget *widget_3_1;
    QPushButton *btncreategroup;

    void setupUi(QWidget *CreateGroup)
    {
        if (CreateGroup->objectName().isEmpty())
            CreateGroup->setObjectName("CreateGroup");
        CreateGroup->resize(460, 600);
        CreateGroup->setMinimumSize(QSize(460, 600));
        CreateGroup->setMaximumSize(QSize(460, 600));
        verticalLayout = new QVBoxLayout(CreateGroup);
        verticalLayout->setSpacing(3);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(3, 3, 3, 3);
        shadowwidget = new QWidget(CreateGroup);
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
        groupclose = new QPushButton(widget_1_1);
        groupclose->setObjectName("groupclose");
        groupclose->setMinimumSize(QSize(0, 35));
        groupclose->setMaximumSize(QSize(16777215, 35));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("\346\245\267\344\275\223")});
        font1.setPointSize(16);
        groupclose->setFont(font1);
        groupclose->setStyleSheet(QString::fromUtf8("#groupclose {\n"
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
"/* \351\274\240\346\240\207\346\202\254\346\265\256\357\274\232\346\265"
                        "\205\344\270\200\345\217\267\347\232\204\347\216\253\347\221\260\347\272\242 */\n"
"#groupclose:hover {\n"
"    background-color: #E02041 !important; /* \346\202\254\346\265\256\346\265\205\347\216\253\347\221\260\347\272\242\357\274\214\346\257\224\351\273\230\350\256\244\350\211\262\346\267\241 */\n"
"	color: #FFFFFF !important; /* \346\202\254\346\265\256\346\227\266\345\255\227\344\275\223\347\231\275\350\211\262 */\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\214\211\344\270\213\357\274\232\351\242\234\350\211\262\345\217\230\346\267\261\357\274\210\346\267\261\344\270\200\345\217\267\347\232\204\347\216\253\347\221\260\347\272\242\357\274\211 */\n"
"#groupclosee:pressed {\n"
"    background-color: #C01838 !important; /* \346\214\211\344\270\213\346\267\261\347\216\253\347\221\260\347\272\242\357\274\214\346\257\224\351\273\230\350\256\244\350\211\262\346\267\261 */\n"
"	color: #FFFFFF !important; /* \346\202\254\346\265\256\346\227\266\345\255\227\344\275\223\347\231\275\350\211\262 */\n"
"}"));

        horizontalLayout->addWidget(groupclose);


        horizontalLayout_2->addWidget(widget_1_1);


        verticalLayout_2->addWidget(widget_1);

        widget_2 = new QWidget(shadowwidget);
        widget_2->setObjectName("widget_2");
        widget_2->setMinimumSize(QSize(0, 200));
        widget_2->setMaximumSize(QSize(16777215, 200));
        widget_2->setStyleSheet(QString::fromUtf8("#widget_2 {\n"
"    background-color: #FFFFFF !important;\n"
"    visibility: visible !important;\n"
"    opacity: 1 !important;\n"
"    border: none !important;\n"
"\n"
"\n"
"}"));
        groupname = new QLineEdit(widget_2);
        groupname->setObjectName("groupname");
        groupname->setGeometry(QRect(70, 130, 311, 41));
        groupname->setFont(font);
        groupname->setStyleSheet(QString::fromUtf8("#groupname {\n"
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
        groupname->setFrame(true);
        btnAvatargroup = new QPushButton(widget_2);
        btnAvatargroup->setObjectName("btnAvatargroup");
        btnAvatargroup->setGeometry(QRect(180, 20, 90, 90));
        QFont font2;
        font2.setPointSize(12);
        btnAvatargroup->setFont(font2);
        btnAvatargroup->setStyleSheet(QString::fromUtf8("#btnAvatargroup {\n"
"    /* \346\240\270\345\277\203\357\274\232\346\267\261\347\201\260\350\211\262(#999999)\350\231\232\347\272\277\350\276\271\346\241\206\357\274\210\346\233\277\344\273\243\345\216\237\346\265\205\347\201\260#F0F0F0\357\274\211 */\n"
"    border: 1px dashed #999999 !important; /* 1px\350\231\232\347\272\277 + \346\267\261\347\201\260\350\211\262\357\274\214\350\247\206\350\247\211\346\233\264\351\206\222\347\233\256 */\n"
"\n"
"    \n"
"    /* \346\226\207\346\234\254\351\242\234\350\211\262\357\274\232\345\220\214\346\255\245\346\224\271\344\270\272\346\267\261\347\201\260\350\211\262#999999 */\n"
"    color: #999999 !important;\n"
"    \n"
"    /* \350\203\214\346\231\257\351\200\217\346\230\216\357\274\214\351\200\202\351\205\215\345\244\232\346\225\260\345\234\272\346\231\257\357\274\210\345\246\202\351\234\200\346\267\261\350\211\262\345\272\225\345\217\257\346\233\277\346\215\242\357\274\211 */\n"
"    background-color: transparent !important;\n"
"    \n"
"    /* \345\206\205\350\276"
                        "\271\350\267\235\357\274\232\351\201\277\345\205\215\346\226\207\345\255\227\350\264\264\350\276\271\346\241\206\357\274\214\346\217\220\345\215\207\346\216\222\347\211\210\350\210\222\351\200\202\345\272\246 */\n"
"    padding: 6px 12px !important;\n"
"    \n"
"    /* \346\270\205\351\231\244QT\351\273\230\350\256\244\347\204\246\347\202\271\346\241\206/\345\244\232\344\275\231\346\240\267\345\274\217 */\n"
"    outline: none !important;\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\202\254\346\265\256\357\274\232\350\231\232\347\272\277+\346\226\207\346\234\254\347\250\215\346\267\261\357\274\214\345\242\236\345\274\272\344\272\244\344\272\222\345\217\215\351\246\210 */\n"
"#btnAvatargroup:hover {\n"
"    border-color: #888888 !important; /* \346\202\254\346\265\256\346\227\266\346\267\261\344\270\200\347\272\247\347\201\260\350\211\262 */\n"
"    color: #888888 !important;\n"
"    background-color: rgba(0, 0, 0, 0.03) !important; /* \350\275\273\345\276\256\350\203\214\346\231\257\347\201\260\357\274\214\344"
                        "\270\215\347\252\201\345\205\200 */\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\214\211\344\270\213\357\274\232\350\231\232\347\272\277+\346\226\207\346\234\254\345\206\215\346\267\261\357\274\214\346\250\241\346\213\237\346\214\211\345\216\213\350\264\250\346\204\237 */\n"
"#btnAvatargroup:pressed {\n"
"    border-color: #777777 !important; /* \346\214\211\344\270\213\346\227\266\346\234\200\346\267\261\344\270\200\347\272\247\347\201\260\350\211\262 */\n"
"    color: #777777 !important;\n"
"}"));

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
        verticalLayout_3 = new QVBoxLayout(widget_3);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        group_widget = new QWidget(widget_3);
        group_widget->setObjectName("group_widget");
        group_widget->setStyleSheet(QString::fromUtf8(""));
        verticalLayout_4 = new QVBoxLayout(group_widget);
        verticalLayout_4->setSpacing(0);
        verticalLayout_4->setObjectName("verticalLayout_4");
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        creategroupList = new QListView(group_widget);
        creategroupList->setObjectName("creategroupList");
        creategroupList->setStyleSheet(QString::fromUtf8("/* \346\273\232\345\212\250\346\235\241\346\225\264\344\275\223\357\274\2327px\345\256\275+\345\205\2500\350\276\271\350\267\235 */\n"
"QListView#creategroupList QScrollBar:vertical {\n"
"    width: 7px;\n"
"    background: transparent;\n"
"    margin: 0px !important;  /* \345\274\272\345\210\266\345\205\2500 */\n"
"    padding: 0px !important; /* \345\274\272\345\210\266\345\205\2500 */\n"
"}\n"
"\n"
"/* \346\273\221\346\247\275\357\274\232\345\205\2500\350\276\271\350\267\235+3px\345\234\206\350\247\222 */\n"
"QListView#creategroupList QScrollBar::groove:vertical {\n"
"    background: transparent;\n"
"    border-radius: 3px;\n"
"    margin: 0px !important;\n"
"    padding: 0px !important;\n"
"}\n"
"\n"
"/* \346\273\221\345\235\227\357\274\232\346\240\270\345\277\203\357\274\201\345\205\2500\350\276\271\350\267\235+3px\345\234\206\350\247\222+\346\227\240\346\214\244\345\216\213 */\n"
"QListView#creategroupList QScrollBar::handle:vertical {\n"
"    background: #C8C8C8;\n"
"    border-radius: 3px;      /* 7px\345"
                        "\256\275\347\224\2503px\345\234\206\350\247\222\357\274\214Qt\346\270\262\346\237\223\346\234\200\345\205\274\345\256\271 */\n"
"    margin: 0px !important;  /* \345\275\273\345\272\225\345\217\226\346\266\210\346\211\200\346\234\211\350\276\271\350\267\235\357\274\214\351\201\277\345\205\215\346\214\244\345\216\213 */\n"
"    padding: 0px !important;\n"
"    min-height: 30px;        /* \346\234\200\345\260\217\351\253\230\345\272\246\350\266\263\345\244\237\357\274\214\344\277\235\350\257\201\345\234\206\350\247\222\346\230\276\347\244\272 */\n"
"}\n"
"\n"
"/* \346\273\221\345\235\227hover\347\212\266\346\200\201 */\n"
"QListView#creategroupList QScrollBar::handle:vertical:hover {\n"
"    background: #A8A8A8;\n"
"}\n"
"\n"
"/* \346\273\221\345\235\227\346\214\211\344\270\213\347\212\266\346\200\201 */\n"
"QListView#creategroupList QScrollBar::handle:vertical:pressed {\n"
"    background: #888888;\n"
"}\n"
"\n"
"/* \351\232\220\350\227\217\347\256\255\345\244\264\357\274\232\345\205\2500\350\276\271\350\267\235"
                        " */\n"
"QListView#creategroupList QScrollBar::sub-line:vertical,\n"
"QListView#creategroupList QScrollBar::add-line:vertical {\n"
"    height: 0px;\n"
"    width: 0px;\n"
"    margin: 0px;\n"
"    padding: 0px;\n"
"}\n"
"\n"
"QListView#creategroupList QScrollBar::up-arrow:vertical,\n"
"QListView#creategroupList QScrollBar::down-arrow:vertical {\n"
"    image: none;\n"
"}\n"
"\n"
"QListView#creategroupList QScrollBar::corner:vertical {\n"
"    background: transparent;\n"
"    margin: 0px;\n"
"    padding: 0px;\n"
"}\n"
"\n"
"#creategroupList {\n"
"    /* 1. \345\275\273\345\272\225\346\227\240\350\276\271\346\241\206\357\274\210\346\240\270\345\277\203\351\234\200\346\261\202\357\274\211 */\n"
"    border: none !important;          /* \345\216\273\346\216\211\345\210\227\350\241\250\345\216\237\347\224\237\350\276\271\346\241\206 */\n"
"    outline: none !important;         /* \345\216\273\346\216\211\347\204\246\347\202\271\351\253\230\344\272\256\346\241\206 */\n"
"    \n"
"    /* 2. \345\276\256\344\277\241\351"
                        "\243\216\346\240\274\350\203\214\346\231\257\350\211\262 */\n"
"    background-color: #FFFFFF !important; /* \345\222\214\345\276\256\344\277\241\346\266\210\346\201\257\345\210\227\350\241\250\350\203\214\346\231\257\344\270\200\350\207\264 */\n"
"    \n"
"    /* 3. \345\210\227\350\241\250\351\241\271\351\227\264\350\267\235\357\274\210\345\217\257\351\200\211\357\274\214\346\250\241\346\213\237\345\276\256\344\277\241\345\210\227\350\241\250\351\241\271\351\227\264\350\267\235\357\274\211 */\n"
"    spacing: 2px !important;\n"
"}\n"
"\n"
"/* \347\276\216\345\214\226\346\273\232\345\212\250\346\235\241\357\274\210\345\276\256\344\277\241\347\252\204\346\273\232\345\212\250\346\235\241\357\274\214\345\217\257\351\200\211\344\275\206\346\216\250\350\215\220\357\274\211 */\n"
"#creategroupList QScrollBar:vertical {\n"
"    background: transparent !important; /* \346\273\232\345\212\250\346\235\241\350\203\214\346\231\257\351\200\217\346\230\216 */\n"
"    width: 6px !important;              /* \347\252\204\346\273"
                        "\232\345\212\250\346\235\241\357\274\214\350\264\264\345\220\210\345\276\256\344\277\241 */\n"
"}\n"
"#creategroupList QScrollBar::handle:vertical {\n"
"    background-color: #D1D1D1 !important; /* \346\273\232\345\212\250\346\235\241\346\273\221\345\235\227\351\242\234\350\211\262 */\n"
"    border-radius: 3px !important;        /* \346\273\221\345\235\227\345\234\206\350\247\222 */\n"
"}\n"
"#creategroupList QScrollBar::handle:vertical:hover {\n"
"    background-color: #BFBFBF !important; /* \351\274\240\346\240\207\346\202\254\346\265\256\346\273\221\345\235\227\345\212\240\346\267\261 */\n"
"}\n"
"#creategroupList QScrollBar::add-page:vertical,\n"
"#creategroupList QScrollBar::sub-page:vertical {\n"
"    background: transparent !important; /* \346\273\232\345\212\250\346\235\241\347\251\272\347\231\275\345\214\272\345\237\237\351\200\217\346\230\216 */\n"
"}\n"
"\n"
"/* ============================================== */\n"
"/* \346\226\260\345\242\236\357\274\232\351\232\220\350\227\217\346\250\252\345\220\221"
                        "\346\273\232\345\212\250\346\235\241\357\274\210\344\273\205\344\277\235\347\225\231\345\236\202\347\233\264\346\273\232\345\212\250\346\235\241\357\274\214\346\240\270\345\277\203\351\234\200\346\261\202\357\274\211 */\n"
"/* ============================================== */\n"
"/* 1. \346\250\252\345\220\221\346\273\232\345\212\250\346\235\241\346\225\264\344\275\223\351\232\220\350\227\217\357\274\232\351\253\230\345\272\246\350\256\276\344\270\2720\357\274\214\345\275\273\345\272\225\344\270\215\346\230\276\347\244\272 */\n"
"#creategroupList QScrollBar:horizontal {\n"
"    height: 0px !important;           /* \346\250\252\345\220\221\346\273\232\345\212\250\346\235\241\351\253\230\345\272\246\350\256\276\344\270\2720\357\274\214\351\232\220\350\227\217\346\234\254\344\275\223 */\n"
"    background: transparent !important; /* \350\203\214\346\231\257\351\200\217\346\230\216\357\274\214\346\227\240\346\256\213\347\225\231 */\n"
"    margin: 0px !important;           /* \345\274\272\345\210\266\350\276\271\350"
                        "\267\235\344\270\2720\357\274\214\346\227\240\345\244\232\344\275\231\347\251\272\347\231\275 */\n"
"    padding: 0px !important;          /* \345\274\272\345\210\266\345\206\205\350\276\271\350\267\235\344\270\2720\357\274\214\346\227\240\345\244\232\344\275\231\347\251\272\347\231\275 */\n"
"}\n"
"\n"
"/* 2. \346\250\252\345\220\221\346\273\232\345\212\250\346\235\241\346\273\221\345\235\227\351\232\220\350\227\217\357\274\232\345\260\272\345\257\270\350\256\276\344\270\2720\357\274\214\346\227\240\346\256\213\347\225\231 */\n"
"#creategroupList QScrollBar::handle:horizontal {\n"
"    width: 0px !important;            /* \346\273\221\345\235\227\345\256\275\345\272\246\350\256\276\344\270\2720\357\274\214\351\232\220\350\227\217\346\273\221\345\235\227 */\n"
"    height: 0px !important;           /* \346\273\221\345\235\227\351\253\230\345\272\246\350\256\276\344\270\2720\357\274\214\351\232\220\350\227\217\346\273\221\345\235\227 */\n"
"    background: transparent !important; /* \350\203\214\346\231\257\351"
                        "\200\217\346\230\216\357\274\214\346\227\240\346\256\213\347\225\231 */\n"
"}\n"
"\n"
"/* 3. \346\250\252\345\220\221\346\273\232\345\212\250\346\235\241\347\251\272\347\231\275\345\214\272\345\237\237\351\200\217\346\230\216\357\274\232\346\227\240\345\244\232\344\275\231\345\272\225\350\211\262 */\n"
"#creategroupList QScrollBar::add-page:horizontal,\n"
"#creategroupList QScrollBar::sub-page:horizontal {\n"
"    background: transparent !important;\n"
"}\n"
"\n"
"/* 4. \346\250\252\345\220\221\346\273\232\345\212\250\346\235\241\347\256\255\345\244\264\351\232\220\350\227\217\357\274\232\345\275\273\345\272\225\346\266\210\351\231\244\346\256\213\347\225\231\345\205\203\347\264\240 */\n"
"#creategroupList QScrollBar::sub-line:horizontal,\n"
"#creategroupList QScrollBar::add-line:horizontal {\n"
"    width: 0px !important;\n"
"    height: 0px !important;\n"
"}\n"
"\n"
"#creategroupList QScrollBar::left-arrow:horizontal,\n"
"#creategroupList QScrollBar::right-arrow:horizontal {\n"
"    image: none !important;\n"
""
                        "}\n"
""));

        verticalLayout_4->addWidget(creategroupList);


        verticalLayout_3->addWidget(group_widget);

        widget_3_1 = new QWidget(widget_3);
        widget_3_1->setObjectName("widget_3_1");
        widget_3_1->setMinimumSize(QSize(0, 80));
        widget_3_1->setMaximumSize(QSize(16777215, 80));
        widget_3_1->setStyleSheet(QString::fromUtf8("#widget_3_1 {\n"
"    /* \346\265\205\347\201\260\350\211\262\350\203\214\346\231\257\357\274\210\345\276\256\344\277\241PC\347\253\257\345\270\270\347\224\250\346\265\205\347\201\260\357\274\211 */\n"
"    background-color: #FFFFFF !important;\n"
"\n"
"    /* \345\267\246\344\270\213\350\247\222\345\217\263\344\270\213\350\247\222\345\234\206\350\247\2228px\357\274\210\344\277\235\346\214\201\345\216\237\345\247\213\345\200\274\357\274\211 */\n"
"    border-bottom-left-radius: 8px !important;\n"
"    border-bottom-radius: 8px !important;\n"
"    /* \345\217\263\344\270\212/\345\217\263\344\270\213\344\270\215\350\256\276\345\234\206\350\247\222\357\274\214\351\273\230\350\256\244\345\260\226\350\247\222\357\274\214\346\227\240\351\234\200\351\242\235\345\244\226\345\206\231 */\n"
"    /* \345\274\272\345\210\266\346\230\276\347\244\272\357\274\214\346\216\222\351\231\244\351\232\220\350\227\217/\351\200\217\346\230\216\345\271\262\346\211\260 */\n"
"    visibility: visible !important;\n"
"    opacity: 1 !impo"
                        "rtant;\n"
"}\n"
""));
        btncreategroup = new QPushButton(widget_3_1);
        btncreategroup->setObjectName("btncreategroup");
        btncreategroup->setGeometry(QRect(70, 10, 311, 51));
        QFont font3;
        font3.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font3.setPointSize(12);
        font3.setBold(false);
        btncreategroup->setFont(font3);
        btncreategroup->setStyleSheet(QString::fromUtf8("#btncreategroup {\n"
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
"#btncreategroup:hover {\n"
"    background-color: #06B058 !important;\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\214\211\344\270\213\357\274\232\345\276\256\344\277\241\347\273\277\346\234\200\346"
                        "\267\261\347\272\247\357\274\210#059E4E\357\274\211\357\274\214\346\250\241\346\213\237\346\214\211\345\216\213\344\270\213\346\262\211\350\264\250\346\204\237 */\n"
"#btncreategroup:pressed {\n"
"    background-color: #059E4E !important;\n"
"}"));

        verticalLayout_3->addWidget(widget_3_1);


        verticalLayout_2->addWidget(widget_3);


        verticalLayout->addWidget(shadowwidget);


        retranslateUi(CreateGroup);

        QMetaObject::connectSlotsByName(CreateGroup);
    } // setupUi

    void retranslateUi(QWidget *CreateGroup)
    {
        CreateGroup->setWindowTitle(QCoreApplication::translate("CreateGroup", "Form", nullptr));
        label_1_2->setText(QCoreApplication::translate("CreateGroup", "   singchat", nullptr));
        groupclose->setText(QCoreApplication::translate("CreateGroup", "\303\227", nullptr));
        groupname->setText(QString());
        groupname->setPlaceholderText(QCoreApplication::translate("CreateGroup", "\347\276\244\350\201\212\345\220\215\347\247\260", nullptr));
        btnAvatargroup->setText(QCoreApplication::translate("CreateGroup", "\347\276\244\350\201\212\345\244\264\345\203\217", nullptr));
        btncreategroup->setText(QCoreApplication::translate("CreateGroup", "\345\210\233\345\273\272\347\276\244\350\201\212", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CreateGroup: public Ui_CreateGroup {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CREATEGROUP_H
