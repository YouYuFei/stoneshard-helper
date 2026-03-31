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
    QByteArray origData;
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
        addAttribute
    };
    InitialSupply() = default;
    InitialSupply(QString n, QString d, int p, QString k):nameKey(n),description(d),point(p),key(k),type(addItem){}
    InitialSupply(QString n, QString d, int p, QString k, double v):nameKey(n),description(d),point(p),key(k),value(v),type(addBuff){}
    InitialSupply(QString n, QString d, int p, QString k, double v, InitialSupplyTpye t):nameKey(n),description(d),point(p),key(k),value(v),type(t){}
    InitialSupplyTpye type;
    QString nameKey;
    QString description;
    int point;
    QString key;
    double value;
    void exec(QJsonObject *character, QJsonArray *array);
    bool isSelected = false;
};
Q_DECLARE_METATYPE(InitialSupply)
class Common : public QObject
{
    Q_OBJECT

public:
    Common(){}
    static QList<CharacterData> getNewCharacterList();
    static QList<InitialSupply> getInitialSupplies();
    static void setInitialSupplies(const CharacterData &characterData, QList<InitialSupply> list);
    static QByteArray fastRead(const QString &fileName);
    static void fastWrite(const QString &fileName, QByteArray data);
    static CharacterData getCharacter(const QString &fileName);
    static void addBuff(QJsonObject *character, QString key, double value);
    static void addItem(QJsonArray *array, const QString &json);

private:
    static QByteArray decodeFile(const QString &fileName);
    static void encodeFile(const QByteArray &jsonStr, const QString &fileName);
    static QByteArray calcMd5(const QString& jsonString, const QString& saveFilePath);
    static QString m_homeDir;
    static QMap<QString, QString> m_nameKeyMap;
    static QList<InitialSupply> m_initialSupplies;
};

#endif // STONESHARDCOMMON_H
