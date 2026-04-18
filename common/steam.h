#ifndef STEAM_H
#define STEAM_H

#include <QObject>

class Steam : public QObject
{
    Q_OBJECT
public:
    Steam(){}
    static int getUnlockedAchievements();
    static int getPlaytime();

private:
    static QString findSteamUserData();
    static QString getLoginId(const QString &path);
    static QString m_userDataPath;
    static QStringList m_steamUrlList;
};

#endif // STEAM_H
