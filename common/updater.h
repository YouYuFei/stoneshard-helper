#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include <QNetworkAccessManager>

class Updater : public QObject
{
    Q_OBJECT
public:
    Updater(QObject *parent = nullptr);
    void checkUpdate();
private:
    void onUpdateCheckFinished();
    void downloadFile();
    QNetworkAccessManager *m_networkManager = nullptr;
    QNetworkReply *m_reply = nullptr;
    QString m_md5;
signals:
    void updateMsg(QString,QString);
};

#endif // UPDATER_H
