#include "common.h"

QString Common::m_homeDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/StoneShard/characters_v1/";
QStringList Common::filterType = {"全部","天赋","资源","战斗","生存","抗性","魔法"};
QMap<QString, QString> Common::m_nameKeyMap = {{"Hilda","希尔达"},{"Arna","阿娜"},{"Velmir","韦尔米尔"},{"Jorgrim","约戈里姆"},{"Dirwin","德温"},{"Jonna","约娜"},{"Leosthenes","琉斯典纳斯"},{"Mahir","玛息尔"}};
QTimer *Common::m_automaticBackup = new QTimer;
QList<InitialSupply> Common::m_initialSupplies = {
    //负面效果
    InitialSupply("不堪一击1","HP最大值-10",1,"max_hp",4,-10),
    InitialSupply("不堪一击2","HP最大值-20",2,"max_hp",4,-20),
    InitialSupply("不堪一击3","HP最大值-30",3,"max_hp",4,-30),
    InitialSupply("精神萎靡1","MP最大值-10",1,"max_mp",4,-10),
    InitialSupply("精神萎靡2","MP最大值-20",2,"max_mp",4,-20),
    InitialSupply("精神萎靡3","MP最大值-30",3,"max_mp",4,-30),
    InitialSupply("不学无术1","技能点-1",1,"SP",2,-1,InitialSupply::addAttribute),
    InitialSupply("弱不禁风1","所受伤害+10%",1,"Damage_Received",5,10),
    InitialSupply("弱不禁风2","所受伤害+20%",2,"Damage_Received",5,20),
    InitialSupply("弱不禁风3","所受伤害+30%",3,"Damage_Received",5,30),
    //战斗
    InitialSupply("神圣加持","神圣伤害+1",-1,"Sacred_Damage",3,1),
    InitialSupply("暗影加持","邪术伤害+1",-1,"Unholy_Damage",3,1),
    InitialSupply("火焰加持","灼烧伤害+1",-1,"Fire_Damage",3,1),
    InitialSupply("冰霜加持","霜冻伤害+1",-1,"Frost_Damage",3,1),
    InitialSupply("闪电加持","电击伤害+1",-1,"Shock_Damage",3,1),
    InitialSupply("灵能加持","灵能伤害+1",-1,"Psionic_Damage",3,1),
    InitialSupply("腐蚀加持","腐蚀伤害+1",-1,"Caustic_Damage",3,1),
    InitialSupply("毒素加持","中毒伤害+1",-1,"Poison_Damage",3,1),
    InitialSupply("秘术加持","秘术伤害+1",-1,"Arcane_Damage",3,1),
    InitialSupply("找准要害1","暴击几率+5%",-1,"CRT",3,5),
    InitialSupply("找准要害2","暴击几率+10%",-2,"CRT",3,10),
    InitialSupply("找准要害3","暴击几率+15%",-3,"CRT",3,15),
    InitialSupply("找准要害4","暴击几率+20%",-4,"CRT",3,20),
    InitialSupply("心狠手辣1","暴击效果+10",-1,"CRTD",3,10),
    InitialSupply("心狠手辣2","暴击效果+20",-2,"CRTD",3,20),
    InitialSupply("心狠手辣3","暴击效果+30",-3,"CRTD",3,30),
    //生存
    InitialSupply("身强体壮1","HP最大值+10",-1,"max_hp",4,10),
    InitialSupply("身强体壮2","HP最大值+20",-2,"max_hp",4,20),
    InitialSupply("身强体壮3","HP最大值+30",-3,"max_hp",4,30),
    InitialSupply("无懈可击1","格挡几率+10%",-1,"PRR",4,10),
    InitialSupply("无懈可击2","格挡几率+20%",-2,"PRR",4,20),
    InitialSupply("无懈可击3","格挡几率+30%",-3,"PRR",4,30),
    InitialSupply("坚如磐石1","格挡力量+10",-1,"Block_Power",4,10),
    InitialSupply("坚如磐石2","格挡力量+20",-2,"Block_Power",4,20),
    InitialSupply("坚如磐石3","格挡力量+30",-3,"Block_Power",4,30),
    InitialSupply("呼吸节奏1","格挡力量恢复+10%",-1,"Block_Recovery",4,10),
    InitialSupply("呼吸节奏2","格挡力量恢复+20%",-2,"Block_Recovery",4,20),
    InitialSupply("呼吸节奏3","格挡力量恢复+30%",-3,"Block_Recovery",4,30),
    InitialSupply("精力充沛1","疲劳抗性+10%",-1,"Fatigue_Gain",4,10),
    InitialSupply("精力充沛2","疲劳抗性+20%",-2,"Fatigue_Gain",4,20),
    InitialSupply("精力充沛3","疲劳抗性+30%",-3,"Fatigue_Gain",4,30),
    InitialSupply("迷踪步法1","闪躲几率+10%",-1,"EVS",4,10),
    InitialSupply("迷踪步法2","闪躲几率+20%",-2,"EVS",4,20),
    InitialSupply("迷踪步法3","闪躲几率+30%",-3,"EVS",4,30),
    InitialSupply("不动如山1","位移抗性+10%",-1,"Knockback_Resistance",4,10),
    InitialSupply("不动如山2","位移抗性+20%",-2,"Knockback_Resistance",4,20),
    InitialSupply("不动如山3","位移抗性+30%",-3,"Knockback_Resistance",4,30),
    //抗性
    InitialSupply("钢筋铁骨1","所受伤害-10%",-1,"Damage_Received",5,-10),
    InitialSupply("钢筋铁骨2","所受伤害-20%",-2,"Damage_Received",5,-20),
    InitialSupply("钢筋铁骨3","所受伤害-30%",-3,"Damage_Received",5,-30),
    //魔法
    InitialSupply("咒法洞悉1","奇观几率+5%",-1,"Miracle_Chance",6,5),
    InitialSupply("咒法洞悉2","奇观几率+10%",-2,"Miracle_Chance",6,10),
    InitialSupply("咒法洞悉3","奇观几率+15%",-3,"Miracle_Chance",6,15),
    InitialSupply("咒法共振1","奇观效果+10",-1,"Miracle_Power",6,10),
    InitialSupply("咒法共振2","奇观效果+20",-2,"Miracle_Power",6,20),
    InitialSupply("咒法共振3","奇观效果+30",-3,"Miracle_Power",6,30),
    //天赋
    InitialSupply("狂猎","希尔达的初始天赋",-3,"o_perk_wild_hunt",1,InitialSupply::addPerks,"希尔达"),
    InitialSupply("无畏誓言","阿娜的初始天赋",-3,"o_perk_vow_feat",1,InitialSupply::addPerks,"阿娜"),
    InitialSupply("复仇心切","韦尔米尔的初始天赋",-3,"o_perk_suum_cuique",1,InitialSupply::addPerks,"韦尔米尔"),
    InitialSupply("血与荣耀","约戈里姆的初始天赋",-3,"o_perk_berserk",1,InitialSupply::addPerks,"约戈里姆"),
    InitialSupply("猎场老手","德温的初始天赋",-3,"o_perk_trained_eye",1,InitialSupply::addPerks,"德温"),
    InitialSupply("魔法学养","约娜的初始天赋",-3,"o_perk_magical_erudition",1,InitialSupply::addPerks,"约娜"),
    InitialSupply("力量与魔法","琉斯典纳斯的初始天赋",-3,"o_perk_might_and_magic",1,InitialSupply::addPerks,"琉斯典纳斯"),
    InitialSupply("终生游历","玛息尔的初始天赋",-3,"o_perk_lifelong_journey",1,InitialSupply::addPerks,"玛息尔"),
    //资源
    InitialSupply("阅历丰富1","技能点+1",-1,"SP",2,1,InitialSupply::addAttribute),
    InitialSupply("阅历丰富2","技能点+2",-2,"SP",2,2,InitialSupply::addAttribute),
    InitialSupply("阅历丰富3","技能点+3",-3,"SP",2,3,InitialSupply::addAttribute),
    InitialSupply("天赋异禀1","属性点+1",-1,"AP",2,1,InitialSupply::addAttribute),
    InitialSupply("天赋异禀2","属性点+2",-2,"AP",2,2,InitialSupply::addAttribute),
    InitialSupply("天赋异禀3","属性点+3",-3,"AP",2,3,InitialSupply::addAttribute),
    InitialSupply("家境殷实","豪华钱包+2500金币",-1,":/json/o_inv_bag_belt01.json",2),
    InitialSupply("盗圣遗物","高耐久的撬棍",-1,":/json/o_inv_tinker.json",2),
    InitialSupply("圣火不灭","永不熄灭的提灯（耐久条过长，建议常驻背包右下角）",-1,":/json/o_inv_lantern.json",2),
    InitialSupply("隐士传承","强化版隐士之戒",-3,":/json/Hermit Ring.json",2)
};

void Common::setInitialSupplies(const CharacterData &characterData, QList<InitialSupply> list)
{
    if (characterData.fileName.isEmpty() || characterData.origData.isEmpty() || list.length() != m_initialSupplies.length()) {
        qDebug()<<"参数不合规" << characterData.fileName << characterData.origData.size();
        return;
    }
    QJsonObject root = characterData.origData;
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
    } else if (type == addPerks) {
        Common::addPerks(character, key);
        if (characterName == "希尔达") {
            Common::addItem(inventory, ":/json/o_inv_hilda_trinket.json");
            QJsonArray arr = character->value("recipesConsumsOpened").toArray();
            arr.append("hilda_trinket");
            character->insert("recipesConsumsOpened", arr);
        }
    }
}

void Common::addBuff(QJsonObject *character, QString key, double value) {
    QJsonArray arr = character->value("buffs").toArray();
    arr.append("o_temp_incr_atr");
    arr.append(-1);
    arr.append(1);
    arr.append("player");
    arr.append(key);
    arr.append(value);
    arr.append(-4);
    arr.append(0);
    character->insert("buffs", arr);
}

void Common::addItem(QJsonArray *array, const QString &json)
{
    QByteArray origData = fastRead(json);
    QJsonArray tmp = QJsonDocument::fromJson(origData).array();
    array->append(tmp);
}

void Common::addPerks(QJsonObject *character, QString key)
{
    QJsonArray arr = character->value("perksList").toArray();
    if (!arr.contains(key)) {
        arr.append(key);
    }
    character->insert("perksList", arr);
}

void Common::automaticBackup()
{
    if (m_automaticBackup->isActive()) {
        return;
    }
    m_automaticBackup->setInterval(3000);
    connect(m_automaticBackup,&QTimer::timeout,[]{
        QStringList dirs = QDir(m_homeDir).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QString &str : dirs) {
            if (!str.startsWith("character_") || str.split("_").last().toInt() == 0) {
                continue;
            }
            QString characterPath = m_homeDir + str;
            QString exitPath = characterPath + "/exitsave_1";
            QStringList childDirs = QDir(m_homeDir + str).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
            bool hasExit = childDirs.contains("exitsave_1");
            QString backPath;
            for (QString tmp : childDirs) {
                if (tmp.contains(".bak.")) {
                    if (backPath.isEmpty()) {
                        backPath = characterPath + "/" + tmp;
                    } else {
                        QDir( characterPath + "/" + tmp).removeRecursively();
                    }
                }
            }
            QString birthTime = QString::number(QFileInfo(exitPath).birthTime().toSecsSinceEpoch());
            if (hasExit && backPath.isEmpty()) {
                backPath = exitPath + ".bak." + birthTime;
                fastCopy(exitPath, backPath);
            } else if (!hasExit && !backPath.isEmpty()) {
                fastCopy(backPath, exitPath);
            } else if (hasExit && !backPath.isEmpty()) {
                QString lastStr = backPath.mid(backPath.lastIndexOf('.') + 1);
                if (birthTime != lastStr) {
                    QDir(backPath).removeRecursively();
                    backPath = exitPath + ".bak." + birthTime;
                    fastCopy(exitPath, backPath);
                }
            }
        }
    });
    m_automaticBackup->start();
}

QList<InitialSupply> Common::getInitialSupplies()
{
    return m_initialSupplies;
}

QString Common::getSaveDir()
{
    return m_homeDir;
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
        QString savePath = characterPath + "/autosave_1/data.sav";
        if (!QFile::exists(savePath)) {
            savePath = characterPath + "/save_1/data.sav";
            if (!QFile::exists(savePath)) {
                continue;
            }
        }
        CharacterData data = getCharacter(savePath);
        result.append(data);
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
    }
#endif
    QByteArray jsonStr = decodeFile(filePath);
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr);
    result.origData = doc.object();
    QJsonObject root = result.origData;
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

void Common::fastCopy(const QString &oldName, const QString &newName)
{
    QFileInfo info(oldName);
    if (info.isFile()) {
        QFile::copy(oldName,newName);
        return;
    }
    QDir().mkpath(newName);
    for (const QString &item : QDir(oldName).entryList(QDir::AllEntries | QDir::NoDotAndDotDot)) {
        QString s = oldName + "/" + item, d = newName + "/" + item;
        fastCopy(s,d);
    }
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
