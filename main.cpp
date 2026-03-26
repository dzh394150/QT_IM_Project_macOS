#include <QApplication>
#include <QSslSocket>
#include <QDir>
#include <QLibrary>
#include <QPluginLoader>
#include <QFileInfo>
#include "login.h"

int main(int argc, char *argv[])
{
    // 开启SSL握手详细日志（定位证书验证失败原因）
    qputenv("QT_LOGGING_RULES", "qt.network.ssl=true;qt.network.ssl.debug=true");
    QApplication a(argc, argv);
    // ====================== SSL验证 ======================
    qDebug() << "\n===================== 【5. SSL功能最终验证】 ====================== ";
    qDebug() << "✅ 编译时OpenSSL版本：" << QSslSocket::sslLibraryBuildVersionString();
    qDebug() << "✅ 运行时OpenSSL版本：" << QSslSocket::sslLibraryVersionString();
    qDebug() << "✅ Qt是否支持SSL：" << QSslSocket::supportsSsl();
    qDebug() << "✅ 可用SSL后端：" << QSslSocket::availableBackends();

    LoginWidget w;
    w.show();
    return a.exec();
}
