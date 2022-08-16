#include <QSettings>
#include <QApplication>
#include "mainwindow.h"

static const QString gLevels[] = {"Debg", "Warn", "Err-", "Err+", "Info", "Syst"};
static void myMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    auto level = gLevels[(int)type];
    QString filePath(context.file);

    auto line = QString("[%1][%2:%3] %4\n").arg(level, filePath).arg(context.line).arg(msg.toLocal8Bit().data());

    fprintf(stdout, "%s", line.toLocal8Bit().data());
    fflush(stdout);

}

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("CVTE");
//    QCoreApplication::setOrganizationDomain("xxx.com");
    QCoreApplication::setApplicationName("log-player");
    QSettings config;
    auto keys = config.allKeys();

    config.setValue("recentFile",QStringList{});
    config.sync();
    qInstallMessageHandler(myMessageHandler);
    QApplication a(argc, argv);


    MainWindow w;
    w.show();
    return a.exec();
}
