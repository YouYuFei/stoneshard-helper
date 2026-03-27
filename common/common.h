#ifndef STONESHARDCOMMON_H
#define STONESHARDCOMMON_H

#include <QObject>
#include <QMap>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>

struct CharacterData{
    QString icon;
    QString nameKey;
    QString fileName;
    QByteArray data;
    QString index;
    double statsTimeLevel;
};

struct InitialSupply{
    InitialSupply(const QString &n, const QString &d, int p):nameKey(n),description(d),point(p){}
    QString nameKey;
    QString description;
    int point;
};

class Common : public QObject
{
    Q_OBJECT

public:
    Common(){}
    static QList<CharacterData> getNewCharacterList();
    static QList<InitialSupply> getInitialSupplies();
    static void setInitialSupplies(const CharacterData &characterData, QList<bool> list);

    static QByteArray fastRead(const QString &fileName);
    static void fastWrite(const QString &fileName, QByteArray data);

private:
    static QByteArray decodeFile(const QString &fileName);
    static void encodeFile(const QByteArray &jsonStr, const QString &fileName);
    static QByteArray calcMd5(const QString& jsonString, const QString& saveFilePath);
    static CharacterData getNewCharacter(const QString &fileName, const QString &characterIndex);
    static QString m_homeDir;
    static QMap<QString, QString> m_nameKeyMap;
    static QList<InitialSupply> m_initialSupplies;
};
Q_DECLARE_METATYPE(CharacterData)
#endif // STONESHARDCOMMON_H
