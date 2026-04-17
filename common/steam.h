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
};

#endif // STEAM_H
