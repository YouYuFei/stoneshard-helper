#include "common.h"
#include <QDir>
#include <QCryptographicHash>
#include <QStandardPaths>

QString Common::m_homeDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/StoneShard/characters_v1/";
QMap<QString, QString> Common::m_nameKeyMap = {{"Hilda","希尔达"},{"Arna","阿娜"},{"Velmir","韦尔米尔"},{"Jorgrim","约戈里姆"},{"Dirwin","德温"},{"Jonna","约娜"},{"Leosthenes","琉斯典纳斯"},{"Mahir","玛息尔"}};
QList<InitialSupply> Common::m_initialSupplies = {
    InitialSupply("不堪一击1","HP最大值-10",1,"max_hp",-10),
    InitialSupply("不堪一击2","HP最大值-20",2,"max_hp",-20),
    InitialSupply("不堪一击3","HP最大值-30",3,"max_hp",-30),
    InitialSupply("精神萎靡1","MP最大值-10",1,"max_mp",-10),
    InitialSupply("精神萎靡2","MP最大值-20",2,"max_mp",-20),
    InitialSupply("精神萎靡3","MP最大值-30",3,"max_mp",-30),
    InitialSupply("找准要害1","暴击几率+5",-1,"CRT",5),
    InitialSupply("找准要害2","暴击几率+10",-2,"CRT",10),
    InitialSupply("找准要害3","暴击几率+15",-3,"CRT",15),
    InitialSupply("心狠手辣1","暴击效果+10",-1,"CRTD",10),
    InitialSupply("心狠手辣2","暴击效果+20",-2,"CRTD",20),
    InitialSupply("心狠手辣3","暴击效果+30",-3,"CRTD",30),
    InitialSupply("身强体壮1","HP最大值+10",-1,"max_hp",10),
    InitialSupply("身强体壮2","HP最大值+20",-2,"max_hp",20),
    InitialSupply("身强体壮3","HP最大值+30",-3,"max_hp",30),
    InitialSupply("神圣加持","神圣伤害+1",-1,"Sacred_Damage",1),
    InitialSupply("暗影加持","邪术伤害+1",-1,"Unholy_Damage",1),
    InitialSupply("火焰加持","灼烧伤害+1",-1,"Fire_Damage",1),
    InitialSupply("冰霜加持","霜冻伤害+1",-1,"Frost_Damage",1),
    InitialSupply("闪电加持","电击伤害+1",-1,"Shock_Damage",1),
    InitialSupply("灵能加持","灵能伤害+1",-1,"Psionic_Damage",1),
    InitialSupply("腐蚀加持","腐蚀伤害+1",-1,"Caustic_Damage",1),
    InitialSupply("毒素加持","中毒伤害+1",-1,"Poison_Damage",1),
    InitialSupply("秘术加持","秘术伤害+1",-1,"Arcane_Damage",1),
    InitialSupply("不眠不休","永久获得精神焕发BUFF",-1,"o_b_fresh",0),
    InitialSupply("阅历丰富1","技能点+1",-1,"SP",1,InitialSupply::addAttribute),
    InitialSupply("阅历丰富2","技能点+2",-2,"SP",2,InitialSupply::addAttribute),
    InitialSupply("阅历丰富3","技能点+3",-3,"SP",3,InitialSupply::addAttribute),
    InitialSupply("天赋异禀1","属性点+1",-1,"AP",1,InitialSupply::addAttribute),
    InitialSupply("天赋异禀2","属性点+2",-2,"AP",2,InitialSupply::addAttribute),
    InitialSupply("天赋异禀3","属性点+3",-3,"AP",3,InitialSupply::addAttribute),
    InitialSupply("家境殷实","豪华钱包+2500金币",-1,":/json/o_inv_bag_belt01.json"),
    InitialSupply("盗圣遗物","高耐久的撬棍",-1,":/json/o_inv_tinker.json"),
    InitialSupply("圣火不灭","永不熄灭的提灯（耐久条过长，建议常驻背包右下角）",-1,":/json/o_inv_lantern.json"),
    InitialSupply("矮人馈赠","希尔达的骨器",-1,":/json/o_inv_hilda_trinket.json")
};

void Common::setInitialSupplies(const CharacterData &characterData, QList<InitialSupply> list)
{
    if (characterData.fileName.isEmpty() || characterData.origData.isEmpty() || list.length() != m_initialSupplies.length()) {
        qDebug()<<"参数不合规" << characterData.fileName << characterData.origData.size();
        return;
    }
    QByteArray origJson = characterData.origData;
    QJsonDocument doc = QJsonDocument::fromJson(origJson);
    QJsonObject root = doc.object();
    QJsonObject character = root.value("characterDataMap").toObject();
    QJsonArray inventory = root.value("inventoryDataList").toArray();
    for (InitialSupply sup : list) {
        sup.exec(&character, &inventory);
    }
    root["characterDataMap"] = character;
    root["inventoryDataList"] = inventory;
    root["yyf"] = true;
    QJsonDocument newDoc(root);
    QByteArray newJson = newDoc.toJson(QJsonDocument::Compact);
#ifndef QT_NO_DEBUG
    if (!QFile::exists(characterData.fileName + ".bak")) {
        QFile::copy(characterData.fileName, characterData.fileName + ".bak");
    }
#endif
    encodeFile(newJson, characterData.fileName);
}

void InitialSupply::exec(QJsonObject *character, QJsonArray *inventory)
{
    if (!isSelected) {
        return;
    }
    if (type == addBuff) {
        Common::addBuff(character, key, value);
    } else if (type == addAttribute) {
        double tmp = character->value(key).toDouble() + value;
        character->insert(key, tmp);
    } else if (type == addItem) {
        Common::addItem(inventory, key);
        if (key == ":/json/o_inv_hilda_trinket.json") {
            QJsonArray arr = character->value("recipesConsumsOpened").toArray();
            arr.append("hilda_trinket");
            character->insert("recipesConsumsOpened", arr);
        }
    }
}

void Common::addBuff(QJsonObject *character, QString key, double value) {
    QJsonArray arr = character->value("buffs").toArray();
    if (value) {
        arr.append("o_temp_incr_atr");
        arr.append(-1);
        arr.append(1);
        arr.append("player");
        arr.append(key);
        arr.append(value);
        arr.append(-4);
        arr.append(0);
    } else {
        arr.append(key);
        arr.append(-1);
        arr.append(1);
        arr.append("player");
        arr.append(-4);
        arr.append(-4);
        arr.append(-4);
        arr.append(0);
    }
    character->insert("buffs", arr);
}

void Common::addItem(QJsonArray *array, const QString &json)
{
    QByteArray origData = fastRead(json);
    QJsonArray tmp = QJsonDocument::fromJson(origData).array();
    array->append(tmp);
}

QList<InitialSupply> Common::getInitialSupplies()
{
    return m_initialSupplies;
}

QList<CharacterData> Common::getNewCharacterList()
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
            CharacterData data = getCharacter(savePath);
            result.append(data);
        }
    }
    return result;
}

CharacterData Common::getCharacter(const QString &fileName)
{
    QString filePath = fileName;
    QStringList pathParts = filePath.split('/');
    if (pathParts.length() < 4) {
        filePath = m_homeDir + filePath;
    }
    CharacterData result;
    result.fileName = filePath;
#ifndef QT_NO_DEBUG
    if (QFile::exists(filePath + ".bak")) {
        filePath += ".bak";
        qDebug()<< "使用备份存档";
    }
#endif
    QByteArray jsonStr = decodeFile(filePath);
    result.origData = jsonStr;
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr);
    QJsonObject root = doc.object();
    result.character = root.value("characterDataMap").toObject();
    result.statsTimeLevel = result.character.value("statsTimeLevel").toDouble();
    result.nameKey = result.character.value("nameKey").toString();
    result.icon = ":/portrait/100px-" + result.nameKey + ".png";
    result.inventory = root.value("inventoryDataList").toArray();
    QString tmp = m_nameKeyMap.value(result.nameKey);
    if (!tmp.isEmpty()) {
        result.nameKey = tmp;
    }
    QString characterIndex = pathParts.at(pathParts.length() - 3);
    result.index = characterIndex.split("_").last();
    if (root.value("yyf").toBool(false)) {
        result.statsTimeLevel += 1;
    }
    return result;
}

QByteArray Common::calcMd5(const QString& jsonString, const QString& saveFilePath) {
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

QByteArray Common::fastRead(const QString &fileName)
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QByteArray data = file.readAll();
    file.close();
    return data;
}

void Common::fastWrite(const QString &fileName, QByteArray data)
{
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    file.write(data);
    file.close();
}

QByteArray Common::decodeFile(const QString &fileName)
{
    QByteArray origData = fastRead(fileName);
    QByteArray prefixed(4, 0);
    prefixed.append(origData);
    QByteArray data = qUncompress(prefixed);
    int lastBrace = data.lastIndexOf('}');
    QByteArray jsonData = data.left(lastBrace + 1);
    return jsonData;
}

void Common::encodeFile(const QByteArray &jsonStr, const QString &fileName)
{
    QByteArray fullData = jsonStr + calcMd5(jsonStr,fileName);
    QByteArray compressed = qCompress(fullData);
    compressed = compressed.mid(4);
    fastWrite(fileName, compressed);
}
