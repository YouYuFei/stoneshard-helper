#ifndef STONESHARDCOMMON_H
#define STONESHARDCOMMON_H

#include <QObject>
#include <QMap>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>
#include <QDir>
#include <QCryptographicHash>
#include <QStandardPaths>
#include <QTimer>
#include <QDateTime>

struct CharacterData{
    QString icon;
    QString nameKey;
    QString fileName;
    QJsonObject origData;
    QJsonObject character;
    QJsonArray inventory;
    QString index;
    double statsTimeLevel;
};
Q_DECLARE_METATYPE(CharacterData)
struct InitialSupply{
    enum InitialSupplyTpye{
        addItem = 0,
        addBuff,
        addAttribute,
        addPerks
    };
    InitialSupply() = default;
    InitialSupply(QString n, QString d, int p, QString k, int f):nameKey(n),description(d),point(p),key(k),filterType(f),type(addItem){}
    InitialSupply(QString n, QString d, int p, QString k, int f, InitialSupplyTpye t, QString c):nameKey(n),description(d),point(p),key(k),filterType(f),type(t),characterName(c){}
    InitialSupply(QString n, QString d, int p, QString k, int f, double v):nameKey(n),description(d),point(p),key(k),filterType(f),value(v),type(addBuff){}
    InitialSupply(QString n, QString d, int p, QString k, int f, double v, InitialSupplyTpye t):nameKey(n),description(d),point(p),key(k),filterType(f),value(v),type(t){}
    InitialSupplyTpye type;
    QString nameKey;
    QString characterName;
    QString description;
    int point;
    QString key;
    double value;
    void exec(QJsonObject *character, QJsonArray *array);
    bool isSelected = false;
    int filterType;
};
Q_DECLARE_METATYPE(InitialSupply)
class Common : public QObject
{
    Q_OBJECT

public:
    Common(){}
    static QString getSaveDir();
    static QList<CharacterData> getNewCharacterList();
    static QList<InitialSupply> getInitialSupplies();
    static void setInitialSupplies(const CharacterData &characterData, QList<InitialSupply> list);
    static QByteArray fastRead(const QString &fileName);
    static void fastWrite(const QString &fileName, QByteArray data);
    static void fastCopy(const QString &oldName, const QString &newName);
    static CharacterData getCharacter(const QString &fileName);
    static void addBuff(QJsonObject *character, QString key, double value);
    static void addItem(QJsonArray *array, const QString &json);
    static void addPerks(QJsonObject *character, QString key);
    static void automaticBackup();
    static QStringList filterType;
private:
    static QByteArray decodeFile(const QString &fileName);
    static void encodeFile(const QByteArray &jsonStr, const QString &fileName);
    static QByteArray calcMd5(const QString& jsonString, const QString& saveFilePath);
    static QString m_homeDir;
    static QMap<QString, QString> m_nameKeyMap;
    static QList<InitialSupply> m_initialSupplies;
    static QTimer *m_automaticBackup;
};

#endif // STONESHARDCOMMON_H
