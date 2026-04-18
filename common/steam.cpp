#include "steam.h"
#include "common.h"
#include <QSettings>
#include <QRegularExpression>

const quint64 STEAMID64_BASE = 76561197960265728ULL;
QStringList Steam::m_steamUrlList = {"C:/Program Files (x86)/Steam/","D:/Program Files (x86)/Steam/","E:/Program Files (x86)/Steam/","F:/Program Files (x86)/Steam/",
                              "C:/Steam/","D:/Steam/","E:/Steam/","F:/Steam/"};
QString Steam::m_userDataPath;

int Steam::getUnlockedAchievements()
{
    if (m_userDataPath.isEmpty()) {
        m_userDataPath = Steam::findSteamUserData();
    }
    if (m_userDataPath.isEmpty()) {
        return 0;
    }
    QString jsonName = m_userDataPath + "/config/librarycache/achievement_progress.json";
    QByteArray jsonData = Common::fastRead(jsonName);
    if (jsonData.isEmpty()) {
        qDebug()<< "成就数据获取失败："<< jsonName;
    }
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    QJsonObject root = doc.object();
    QJsonArray apps = root.value("mapCache").toArray();
    for (int i=0;i<apps.size();i++) {
        QJsonArray tmp = apps.at(i).toArray();
       if (tmp.size() < 2 || tmp.at(0).toDouble() != 625960) {
           continue;
       }
       return tmp.at(1).toObject().value("unlocked").toDouble();
    }
    return 0;
}

int Steam::getPlaytime()
{
    if (m_userDataPath.isEmpty()) {
        m_userDataPath = Steam::findSteamUserData();
    }
    if (m_userDataPath.isEmpty()) {
        return 0;
    }
    QString vdfName = m_userDataPath + "/config/localconfig.vdf";
    QByteArray vdfData = Common::fastRead(vdfName);
    if (vdfData.isEmpty()) {
        qDebug()<< "时长数据获取失败："<< vdfName;
    }
    QByteArrayList lines = vdfData.split('\n');
    bool findStineShard = false;
    for (int i = 0;i< lines.size();i++) {
        if (!findStineShard && lines.at(i) == "\t\t\t\t\t\"625960\"") {
            findStineShard = true;
            continue;
        }
        if (findStineShard) {
            if (lines.at(i) == "\t\t\t\t\t}") {
                return 0;
            }
            QByteArray line = lines.at(i).toLower();
            if (line.startsWith("\t\t\t\t\t\t\"playtime\"")) {
                line = line.mid(19);
                line.chop(1);
                return line.toInt();
            }
        }
    }
    return 0;
}

QString Steam::findSteamUserData()
{
    QString steamPath;
    QSettings regSteam("HKEY_CURRENT_USER\\Software\\Valve\\Steam", QSettings::NativeFormat);
    steamPath = regSteam.value("SteamPath").toString();
#ifndef QT_NO_DEBUG
    if (steamPath.isEmpty()) {
        steamPath = Common::getSaveDir();
    }
#endif
    if (steamPath.isEmpty() || !QDir(steamPath).exists()) {
        qDebug()<< "注册表获取主目录失败:" << steamPath;
        for (QString url : m_steamUrlList) {
            if (QDir(url).exists()) {
                steamPath = url;
                qDebug()<< "猜测目录成功："<< steamPath;
                break;
            }
        }
    }
    if (steamPath.isEmpty() || !QDir(steamPath).exists()) {
        qDebug()<< "获取主目录失败:" << steamPath;
    }
    if (!steamPath.endsWith("/")) {
        steamPath.append("/");
    }
    QString userDataPath = steamPath + "userdata/";
    QString userData = userDataPath + getLoginId(steamPath);
    qDebug()<< "最近登录的账号：" << userData;
    QString jsonName = userData + "/config/librarycache/achievement_progress.json";
    if (QFile::exists(jsonName)) {
        return userData;
    }
    qDebug()<< "最近登录的账号不存在成就数据";
    QStringList dirs = QDir(userDataPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (QString dir : dirs) {
        userData = userDataPath + dir;
        jsonName = userData +  "/config/librarycache/achievement_progress.json";
        if (QFile::exists(jsonName)) {
            qDebug()<< "采用首个存在成就数据的账号:" << userData;
            return userData;
        }
    }
    qDebug()<< "未找到任何存在成就数据的账号";
    return "";
}

QString Steam::getLoginId(const QString &path)
{
    QByteArray loginData = Common::fastRead(path + "config/loginusers.vdf");
    if (loginData.isEmpty()) {
        qDebug()<< "登录数据不存在";
        return "";
    }
    QString longId;
    QRegularExpression re(
                R"REGEX("(\d+)"\s*\{[^}]*?"mostrecent"\s+"1")REGEX",
                QRegularExpression::DotMatchesEverythingOption | QRegularExpression::InvertedGreedinessOption | QRegularExpression::CaseInsensitiveOption
                );
    QRegularExpressionMatch match = re.match(loginData);
    if (match.hasMatch()) {
        longId = match.captured(1);
    }
    quint64 steamID64 = longId.toULongLong();
    if (steamID64 < STEAMID64_BASE) {
        return "";
    }
    quint32 steamID32 = static_cast<quint32>((steamID64 - STEAMID64_BASE));
    return QString::number(steamID32);
}
