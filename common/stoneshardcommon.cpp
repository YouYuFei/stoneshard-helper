#include "stoneshardcommon.h"
#include <QDir>
#include <QCryptographicHash>
#include <QStandardPaths>

QString StoneShardCommon::m_homeDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/StoneShard/characters_v1/";
QMap<QString, QString> StoneShardCommon::m_nameKeyMap = {{"Hilda","希尔达"},{"Arna","阿娜"},{"Velmir","韦尔米尔"},{"Jorgrim","约戈里姆"},{"Dirwin","德温"},{"Jonna","约娜"},{"Leosthenes","琉斯典纳斯"},{"Mahir","玛息尔"}};
QList<InitialSupply> StoneShardCommon::m_initialSupplies = {InitialSupply("家境殷实","豪华钱包+2500金币",1),
                                                            InitialSupply("盗圣遗物","高耐久的撬棍",1),
                                                            InitialSupply("资历丰富","技能点+3",3),
                                                            InitialSupply("天赋异禀","属性点+3",3),
                                                            InitialSupply("矮人圣器","希尔达的骨器",2),
                                                            InitialSupply("昆仑灵宝","附带元素攻击的盾牌",4),
                                                            InitialSupply("隐士传承","强化版隐士之戒",4)
                                                           };

void StoneShardCommon::setInitialSupplies(const CharacterData &characterData, QList<bool> list)
{
    if (characterData.fileName.isEmpty() || characterData.data.isEmpty() || list.length() != m_initialSupplies.length()) {
        qDebug()<<"参数不合规" << characterData.fileName << characterData.data.size();
        return;
    }
    QByteArray origJson = characterData.data;
    QJsonDocument doc = QJsonDocument::fromJson(origJson);
    QJsonObject root = doc.object();
    QJsonObject character = root.value("characterDataMap").toObject();
    QJsonArray inventory = root.value("inventoryDataList").toArray();
    if (list.at(0)) {
        QByteArray origData = fastRead(":/o_inv_bag_belt01.json");
        QJsonArray array = QJsonDocument::fromJson(origData).array();
        inventory.append(array);
    }
    if (list.at(1)) {
        QByteArray origData = fastRead(":/o_inv_tinker.json");
        QJsonArray array = QJsonDocument::fromJson(origData).array();
        inventory.append(array);
    }
    if (list.at(2)) {
        character["SP"] = 5;
    }
    if (list.at(3)) {
        character["AP"] = 3;
    }
    if (list.at(4)) {
        QByteArray origData = fastRead(":/o_inv_hilda_trinket.json");
        QJsonArray array = QJsonDocument::fromJson(origData).array();
        inventory.append(array);
    }
    if (list.at(5)) {
        QByteArray origData = fastRead(":/Orient Shield.json");
        QJsonArray array = QJsonDocument::fromJson(origData).array();
        inventory.append(array);
    }
    if (list.at(6)) {
        QByteArray origData = fastRead(":/Hermit Ring.json");
        QJsonArray array = QJsonDocument::fromJson(origData).array();
        inventory.append(array);
    }
    root["characterDataMap"] = character;
    root["inventoryDataList"] = inventory;
    root["yyf"] = true;
    QJsonDocument newDoc(root);
    QByteArray newJson = newDoc.toJson(QJsonDocument::Compact);
    encodeFile(newJson, characterData.fileName);
}

QList<InitialSupply> StoneShardCommon::getInitialSupplies()
{
    return m_initialSupplies;
}

QList<CharacterData> StoneShardCommon::getNewCharacterList()
{
    QList<CharacterData> result;
    QDir dir(m_homeDir);
    QStringList dirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &str : dirs) {
        if (!str.startsWith("character_") || str.split("_").last().toInt() == 0) {
            continue;
        }
        QString characterPath = m_homeDir + str;
        QDir character(characterPath);
        QStringList tmpCharacters = character.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QString &strCharacter : tmpCharacters) {
            QString savePath = characterPath + "/" + strCharacter + "/data.sav";
            if (strCharacter != "autosave_1"|| !QFile::exists(savePath)) {
                continue;
            }
            CharacterData data = getNewCharacter(savePath, str);
            result.append(data);
        }
    }
    return result;
}

CharacterData StoneShardCommon::getNewCharacter(const QString &fileName, const QString &characterIndex)
{
    CharacterData result;
    result.fileName = fileName;
    QByteArray jsonStr = decodeFile(fileName);
    result.data = jsonStr;
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr);
    QJsonObject root = doc.object();
    QJsonObject character = root.value("characterDataMap").toObject();
    result.statsTimeLevel = character.value("statsTimeLevel").toDouble();
    result.nameKey = character.value("nameKey").toString();
    result.icon = ":/portrait/100px-" + result.nameKey + ".png";
    QString tmp = m_nameKeyMap.value(result.nameKey);
    if (!tmp.isEmpty()) {
        result.nameKey = tmp;
    }
    result.index = characterIndex.split("_").last();
    if (root.value("yyf").toBool(false)) {
        result.statsTimeLevel += 1;
    }
    return result;
}

QByteArray StoneShardCommon::calcMd5(const QString& jsonString, const QString& saveFilePath) {
    QByteArray result;
    QString normalizedPath = saveFilePath;
    QStringList pathParts = normalizedPath.split('/');
    if (pathParts.size() < 3) {
        qDebug() << "路径不足3部分，无法获取所需路径段";
        return result;
    }
    QString salt = QString("stOne!characters_v1!%1!%2!shArd")
                       .arg(pathParts[pathParts.size() - 3])
                       .arg(pathParts[pathParts.size() - 2]);
    QByteArray data = (jsonString + salt).toUtf8();
    QByteArray md5Hash = QCryptographicHash::hash(data, QCryptographicHash::Md5);
    result = md5Hash.toHex().append('\x00');
    return result;
}

QByteArray StoneShardCommon::fastRead(const QString &fileName)
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QByteArray data = file.readAll();
    file.close();
    return data;
}

void StoneShardCommon::fastWrite(const QString &fileName, QByteArray data)
{
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    file.write(data);
    file.close();
}

QByteArray StoneShardCommon::decodeFile(const QString &fileName)
{
    QByteArray origData = fastRead(fileName);
    QByteArray prefixed(4, 0);
    prefixed.append(origData);
    QByteArray data = qUncompress(prefixed);
    int lastBrace = data.lastIndexOf('}');
    QByteArray jsonData = data.left(lastBrace + 1);
    return jsonData;
}

void StoneShardCommon::encodeFile(const QByteArray &jsonStr, const QString &fileName)
{
    QByteArray fullData = jsonStr + calcMd5(jsonStr,fileName);
    QByteArray compressed = qCompress(fullData);
    compressed = compressed.mid(4);
    fastWrite(fileName, compressed);
}
