#include "updater.h"
#include "common.h"
#include <QNetworkReply>
#include <QCoreApplication>
#include <QProcess>
#include <QDir>

Updater::Updater(QObject *parent): QObject(parent)
{
    m_networkManager = new QNetworkAccessManager(this);
    checkUpdate();
}

void Updater::checkUpdate()
{
    QUrl versionUrl("http://yyf.luxe/stoneshard-helper/version");
    m_reply = m_networkManager->get(QNetworkRequest(versionUrl));
    connect(m_reply, &QNetworkReply::finished, this, &Updater::onUpdateCheckFinished);
}

void Updater::onUpdateCheckFinished()
{
    QByteArray versionJson = m_reply->readAll();
    m_reply->close();
    if (versionJson.isEmpty()) {
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(versionJson);
    QJsonObject obj = doc.object();
    QString notes = obj["notes"].toString();
    m_md5 = obj["md5"].toString();
    if (notes.isEmpty() || m_md5.isEmpty()) {
        qDebug()<<"version json error :"<<versionJson;
        return;
    }
    QByteArray exe = Common::fastRead(QCoreApplication::applicationFilePath());
    QString md5 = QCryptographicHash::hash(exe, QCryptographicHash::Md5).toHex();
    if (md5 == m_md5) {
        return;
    }
    emit updateMsg("检测到新版本，自动更新中，鼠标悬停此处查看更新内容", notes);
    qDebug()<<"检查更新完成" << md5 << m_md5;
    QString path = QCoreApplication::applicationDirPath();
    while (path.endsWith("/")) {
        path.chop(1);
    }
    QStringList parts = path.split("/");
    QString lastDir = parts.last().toLower();
    if (lastDir.contains("qt")) {
        qDebug()<<"开发环境，跳过后续更新步骤";
        return;
    }
    QUrl versionUrl("http://yyf.luxe/stoneshard-helper/update/stoneshard-helper.exe");
    m_reply = m_networkManager->get(QNetworkRequest(versionUrl));
    connect(m_reply, &QNetworkReply::finished, this, &Updater::downloadFile);
}

void Updater::downloadFile()
{
    QByteArray fileData = m_reply->readAll();
    m_reply->close();
    QString md5 = QCryptographicHash::hash(fileData, QCryptographicHash::Md5).toHex();
    if (md5 != m_md5) {
        emit updateMsg("更新失败，请联系作者修复", m_md5 + "|" + md5);
        return;
    }
    QString tmpName = QCoreApplication::applicationFilePath() + ".tmp";
    QFile::remove(tmpName);
    Common::fastWrite(tmpName, fileData);
    QString batName = "update.bat";
    QFile::remove(batName);
    QFile::copy(":/update.bat",batName);
    QProcess::startDetached("cmd.exe", {"/c", batName});
    emit updateMsg("更新完成，重启后生效", "");
}
