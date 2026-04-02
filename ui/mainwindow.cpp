#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "common.h"
#include <QStandardItemModel>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
#ifndef QT_NO_DEBUG
    m_pointMax += 90;
    //仅调试用
    CharacterData character = Common::getCharacter("character_2/exitsave_1/data.sav");
    for (int i=0; i<character.inventory.size(); i++) {
        QJsonArray item = character.inventory.at(i).toArray();
        QJsonObject itemInfo = item.at(1).toObject();
        if (itemInfo.value("idName") == "tinker") {
            qDebug()<< item;
        }
        qDebug()<<itemInfo.value("charge");
    }
    // for (QString key : character.character.keys()) {
    //     if (key != "Books_Read") {
    //         continue;
    //     }
    //     qDebug()<< key << character.character.value(key);
    // }
    // QJsonObject root = character.origData;
    // QJsonDocument newDoc(root);
    // QByteArray newJson = newDoc.toJson(QJsonDocument::Indented);
    // Common::fastWrite("save_2.json",newJson);
#endif
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    m_updater = new Updater(this);
    connect(m_updater,&Updater::updateMsg,this,&MainWindow::updateMsg);
    setWindowTitle("stoneshard helper");
    setWindowIcon(QIcon(":/icon.png"));
    ui->pushButton_2->setEnabled(false);
    ui->statusbar->showMessage("程序完全免费，无论从任何渠道购买，速速申请退款");
    ui->statusbar->setStyleSheet("QStatusBar{ color: red; font-weight: bold;}");
    ui->label->setText("剩余点数:"+QString::number(m_pointMax));
    on_pushButton_clicked();
    QAction *act;
    act = menuBar()->addAction("",[]{
        QDesktopServices::openUrl(QUrl("https://space.bilibili.com/107071365"));
    });
    act->setIcon(QIcon(":/bilibili.png"));
    act = menuBar()->addAction("",[]{
        QDesktopServices::openUrl(QUrl("https://github.com/YouYuFei/stoneshard-helper"));
    });
    act->setIcon(QIcon(":/github.png"));
    act = menuBar()->addAction("",[]{
        QDesktopServices::openUrl(QUrl("https://yyf.luxe/stoneshard-helper/support"));
    });
    act->setIcon(QIcon(":/support.png"));
    QList<InitialSupply> list = Common::getInitialSupplies();
    QStringList headers = {"名称", "描述", "点数"};
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setRowCount(list.size());
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    for (int i=0; i<list.size(); i++ ) {
        InitialSupply sup = list.at(i);
        QTableWidgetItem *item = new QTableWidgetItem(sup.nameKey);
        QVariant itemData = QVariant::fromValue(sup);
        item->setData(Qt::UserRole + 1, itemData);
        ui->tableWidget->setItem(i, 0, item);
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(sup.description));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(sup.point)));
    }
    on_comboBox_currentTextChanged(ui->comboBox->currentText());
}

void MainWindow::on_pushButton_clicked()
{
    ui->comboBox->clear();
    bool hasNewSave = false;
    QList<CharacterData> list = Common::getNewCharacterList();
    QStandardItemModel *model = new QStandardItemModel(ui->comboBox);
    for (CharacterData data : list) {
        QStandardItem *item = new QStandardItem();
        item->setIcon(QIcon(data.icon));
        item->setText(data.nameKey + "(" + data.index + ")");
        QVariant itemData = QVariant::fromValue(data);
        item->setData(itemData);
        if (data.statsTimeLevel > 1) {
#ifdef QT_NO_DEBUG
            item->setEnabled(false);
#endif
            item->setToolTip("只对新存档开放");
        } else {
            hasNewSave = true;
        }
        model->appendRow(item);
    }
    ui->comboBox->setModel(model);
    if (!hasNewSave) {
        ui->statusbar->showMessage("只对新存档开放，请在游戏中创建角色后，点击刷新按钮");
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    QList<InitialSupply> list;
    for (int i=0; i<ui->tableWidget->rowCount(); i++) {
        QTableWidgetItem* item = ui->tableWidget->item(i,0);
        InitialSupply sup = item->data(Qt::UserRole + 1).value<InitialSupply>();
        sup.isSelected = item->isSelected();
        list.append(sup);
    }
    QModelIndex modelIndex = ui->comboBox->model()->index(ui->comboBox->currentIndex(),0);
    CharacterData data = ui->comboBox->model()->itemData(modelIndex).value(Qt::UserRole + 1).value<CharacterData>();
    Common::setInitialSupplies(data,list);
    on_pushButton_clicked();
}

void MainWindow::on_tableWidget_itemSelectionChanged()
{
    int usePoint = 0;
    for (int i=0; i<ui->tableWidget->rowCount(); i++) {
        QTableWidgetItem* item = ui->tableWidget->item(i,0);
        if(item->isSelected()) {
            usePoint -= ui->tableWidget->item(i,2)->text().toInt();
        }
    }
    int remaining = m_pointMax-usePoint;
    ui->label->setText("配置点:"+QString::number(remaining));
    if (remaining < 0) {
        ui->pushButton_2->setEnabled(false);
    } else {
        ui->pushButton_2->setEnabled(true);
    }
}

void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
{
    QModelIndex modelIndex = ui->comboBox->model()->index(ui->comboBox->currentIndex(),0);
    CharacterData data = ui->comboBox->model()->itemData(modelIndex).value(Qt::UserRole + 1).value<CharacterData>();
    for (int i=0; i<ui->tableWidget->rowCount(); i++) {
        QTableWidgetItem *item0 = ui->tableWidget->item(i,0);
        QTableWidgetItem *item1 = ui->tableWidget->item(i,1);
        QTableWidgetItem *item2 = ui->tableWidget->item(i,2);
        item0->setSelected(false);
        item1->setSelected(false);
        item2->setSelected(false);
        InitialSupply sup = item0->data(Qt::UserRole + 1).value<InitialSupply>();
        Qt::ItemFlags flags = item0->flags() | Qt::ItemIsEnabled;
        if (arg1.isEmpty() || data.nameKey == sup.characterName ) {
            flags = item0->flags() & ~Qt::ItemIsEnabled;
        }
        item0->setFlags(flags);
        item1->setFlags(flags);
        item2->setFlags(flags);
    }
}

void MainWindow::updateMsg(QString msg, QString note)
{
    ui->statusbar->showMessage(msg);
    ui->statusbar->setToolTip(note);
}


