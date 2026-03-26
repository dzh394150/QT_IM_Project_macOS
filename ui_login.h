/********************************************************************************
** Form generated from reading UI file 'login.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGIN_H
#define UI_LOGIN_H

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

class Ui_LoginWidget
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
    QLineEdit *leUsername;
    QLineEdit *lePassword;
    QLabel *label;
    QPushButton *btnRegister;
    QWidget *widget_3;
    QPushButton *login_botton;

    void setupUi(QWidget *LoginWidget)
    {
        if (LoginWidget->objectName().isEmpty())
            LoginWidget->setObjectName("LoginWidget");
        LoginWidget->resize(400, 320);
        LoginWidget->setStyleSheet(QString::fromUtf8(""));
        verticalLayout = new QVBoxLayout(LoginWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(5, 5, 5, 5);
        shadowwidget = new QWidget(LoginWidget);
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
        leUsername = new QLineEdit(widget_2);
        leUsername->setObjectName("leUsername");
        leUsername->setGeometry(QRect(40, 60, 311, 41));
        leUsername->setFont(font);
        leUsername->setStyleSheet(QString::fromUtf8("#leUsername {\n"
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
        leUsername->setFrame(true);
        lePassword = new QLineEdit(widget_2);
        lePassword->setObjectName("lePassword");
        lePassword->setGeometry(QRect(40, 120, 311, 41));
        lePassword->setFont(font);
        lePassword->setStyleSheet(QString::fromUtf8("#lePassword {\n"
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
        label = new QLabel(widget_2);
        label->setObjectName("label");
        label->setGeometry(QRect(50, 10, 301, 31));
        QFont font2;
        font2.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font2.setPointSize(18);
        label->setFont(font2);
        btnRegister = new QPushButton(widget_2);
        btnRegister->setObjectName("btnRegister");
        btnRegister->setGeometry(QRect(40, 170, 111, 18));
        QFont font3;
        font3.setPointSize(12);
        btnRegister->setFont(font3);
        btnRegister->setStyleSheet(QString::fromUtf8("#btnRegister {\n"
"    /* \345\237\272\347\241\200\346\240\267\345\274\217\357\274\232\346\227\240\350\276\271\346\241\206 + \345\233\233\350\247\222\345\234\206\350\247\222 + \351\200\217\346\230\216\350\203\214\346\231\257 */\n"
"    border: none !important;                /* \345\275\273\345\272\225\351\232\220\350\227\217\350\276\271\346\241\206 */\n"
"    border-radius: 4px !important;          /* \345\233\233\350\247\222\345\234\206\350\247\222\357\274\210\345\200\274\345\217\257\346\214\211\351\234\200\350\260\203\346\225\264\357\274\211 */\n"
"    background: transparent !important;     /* \345\210\235\345\247\213\351\200\217\346\230\216 */\n"
"    outline: none !important;               /* \345\216\273\346\216\211\347\204\246\347\202\271\346\241\206\357\274\210QT\351\273\230\350\256\244\347\202\271\345\207\273\345\220\216\344\274\232\346\234\211\357\274\211 */\n"
"    \n"
"    /* \346\226\260\345\242\236\357\274\232\346\226\207\345\255\227\351\235\240\345\267\246 + \346\265\205\347\201\260\350\211\262"
                        " + \345\267\246\345\206\205\350\276\271\350\267\235\357\274\210\351\201\277\345\205\215\346\226\207\345\255\227\350\264\264\350\276\271\357\274\211 */\n"
"    text-align: left !important;            /* \346\226\207\345\255\227\351\235\240\345\267\246\345\257\271\351\275\220 */\n"
"    color: #999999 !important;              /* \346\265\205\347\201\260\350\211\262\346\226\207\345\255\227\357\274\210\345\217\257\346\214\211\351\234\200\350\260\203\346\225\264\357\274\232#AAAAAA\346\233\264\346\265\205 / #888888\347\250\215\346\267\261\357\274\211 */\n"
"    padding-left: 8px !important;           /* \345\267\246\345\206\205\350\276\271\350\267\235\357\274\210\345\217\257\351\200\211\357\274\214\345\200\274\345\217\257\350\260\203\346\225\264\357\274\214\345\246\20210px\357\274\211 */\n"
"    padding-right: 8px !important;          /* \345\217\263\345\206\205\350\276\271\350\267\235\357\274\210\344\277\235\346\214\201\345\257\271\347\247\260\357\274\214\345\217\257\351\200\211\357\274\211 */\n"
"}\n"
"\n"
"/* \351"
                        "\274\240\346\240\207\346\202\254\346\265\256\357\274\232\350\203\214\346\231\257\345\212\240\346\267\261 + \346\226\207\345\255\227\351\242\234\350\211\262\350\275\273\345\276\256\345\212\240\346\267\261\357\274\210\345\217\257\351\200\211\357\274\214\345\242\236\345\274\272\344\272\244\344\272\222\357\274\211 */\n"
"#btnRegister:hover {\n"
"    background-color: #DCDCDC !important;\n"
"    color: #888888 !important;              /* \346\202\254\346\265\256\346\227\266\346\226\207\345\255\227\347\250\215\346\267\261\357\274\210\345\217\257\345\210\240\351\231\244\357\274\214\344\277\235\346\214\201\345\222\214\345\237\272\347\241\200\350\211\262\344\270\200\350\207\264\357\274\211 */\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\214\211\344\270\213\357\274\232\350\203\214\346\231\257\346\233\264\346\267\261 + \346\226\207\345\255\227\351\242\234\350\211\262\345\206\215\346\267\261\344\270\200\347\202\271\357\274\210\345\217\257\351\200\211\357\274\211 */\n"
"#btnRegister:pressed {\n"
"    background-color: #C"
                        "CCCCC !important;\n"
"    color: #777777 !important;            /* \346\214\211\344\270\213\346\227\266\346\226\207\345\255\227\346\233\264\346\267\261\357\274\210\345\217\257\345\210\240\351\231\244\357\274\211 */\n"
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
        login_botton = new QPushButton(widget_3);
        login_botton->setObjectName("login_botton");
        login_botton->setGeometry(QRect(40, 10, 311, 51));
        QFont font4;
        font4.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font4.setPointSize(12);
        font4.setBold(false);
        login_botton->setFont(font4);
        login_botton->setStyleSheet(QString::fromUtf8("#login_botton {\n"
"    /* \342\221\240 \345\275\273\345\272\225\351\232\220\350\227\217\350\276\271\346\241\206\357\274\210\345\214\205\346\213\254\351\273\230\350\256\244\347\232\204\347\204\246\347\202\271\350\276\271\346\241\206\357\274\211 */\n"
"    border: none !important;\n"
"    outline: none !important;\n"
"    \n"
"    /* \342\221\241 \345\234\206\350\247\222\347\237\251\345\275\242\357\274\21012px\346\230\257\345\276\256\344\277\241\350\276\223\345\205\245\346\241\206\345\270\270\347\224\250\345\234\206\350\247\222\357\274\214\345\217\257\346\214\211\351\234\200\350\260\203\346\225\264\357\274\211 */\n"
"    border-radius: 12px !important;\n"
"    \n"
"    background-color: #E02041 !important;\n"
"    /* \347\216\260\344\273\243\344\272\256\347\216\253\347\221\260\347\272\242\357\274\210\346\233\264\350\264\264\350\277\221\347\247\273\345\212\250\347\253\257\345\270\270\347\224\250\351\243\216\346\240\274\357\274\211\n"
"    background-color: #E02041 !important;\n"
"    */\n"
"    \n"
"    /* \345\255"
                        "\227\344\275\223\351\242\234\350\211\262\350\256\276\344\270\272\347\272\257\347\231\275\350\211\262 */\n"
"    color: #FFFFFF !important;\n"
"    \n"
"    /* \344\274\230\345\214\226\357\274\232\345\206\205\350\276\271\350\267\235\357\274\214\351\201\277\345\205\215\346\226\207\345\255\227\350\264\264\345\234\206\350\247\222\350\276\271\347\274\230\357\274\210\345\217\257\351\200\211\344\275\206\346\216\250\350\215\220\357\274\211 */\n"
"    padding: 6px 12px !important;\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\202\254\346\265\256\357\274\232\351\242\234\350\211\262\345\217\230\346\267\241\357\274\210\346\265\205\344\270\200\345\217\267\347\232\204\347\216\253\347\221\260\347\272\242\357\274\211 */\n"
"#login_botton:hover {\n"
"    background-color: #F14A68 !important; /* \346\202\254\346\265\256\346\265\205\347\216\253\347\221\260\347\272\242\357\274\214\346\257\224\351\273\230\350\256\244\350\211\262\346\267\241 */\n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\214\211\344\270\213\357\274\232\351\242\234"
                        "\350\211\262\345\217\230\346\267\261\357\274\210\346\267\261\344\270\200\345\217\267\347\232\204\347\216\253\347\221\260\347\272\242\357\274\211 */\n"
"#login_botton:pressed {\n"
"    background-color: #C01838 !important; /* \346\214\211\344\270\213\346\267\261\347\216\253\347\221\260\347\272\242\357\274\214\346\257\224\351\273\230\350\256\244\350\211\262\346\267\261 */\n"
"}"));

        verticalLayout_2->addWidget(widget_3);


        verticalLayout->addWidget(shadowwidget);


        retranslateUi(LoginWidget);

        QMetaObject::connectSlotsByName(LoginWidget);
    } // setupUi

    void retranslateUi(QWidget *LoginWidget)
    {
        LoginWidget->setWindowTitle(QCoreApplication::translate("LoginWidget", "Form", nullptr));
        label_1_2->setText(QCoreApplication::translate("LoginWidget", "   singchat", nullptr));
        close_botton->setText(QCoreApplication::translate("LoginWidget", "\303\227", nullptr));
        leUsername->setText(QString());
        leUsername->setPlaceholderText(QCoreApplication::translate("LoginWidget", "\350\264\246\345\217\267", nullptr));
        lePassword->setText(QString());
        lePassword->setPlaceholderText(QCoreApplication::translate("LoginWidget", "\345\257\206\347\240\201", nullptr));
        label->setText(QCoreApplication::translate("LoginWidget", "\350\264\246\345\217\267\345\257\206\347\240\201\347\231\273\345\275\225", nullptr));
        btnRegister->setText(QCoreApplication::translate("LoginWidget", "\346\263\250\345\206\214\350\264\246\345\217\267", nullptr));
        login_botton->setText(QCoreApplication::translate("LoginWidget", "\347\231\273\345\275\225", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginWidget: public Ui_LoginWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGIN_H
