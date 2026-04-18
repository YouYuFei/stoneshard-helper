#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

void logToFile(QtMsgType, const QMessageLogContext&, const QString &msg)
{
    static QFile file(QCoreApplication::applicationDirPath() + "/log.txt");
    if (!file.isOpen()) {
        file.remove();
        file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    }
    QString message = QString("[%1]%2").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd-hh:mm:ss")).arg(msg);
    QTextStream out(&file);
    out << message << '\n';
}

int main(int argc, char *argv[])
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QApplication a(argc, argv);
#ifdef QT_NO_DEBUG
    qInstallMessageHandler(logToFile);
#endif
    MainWindow w;
    w.show();
    return a.exec();
}
