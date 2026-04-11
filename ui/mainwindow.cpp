#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "common.h"
#include "speed.h"
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
    CharacterData character = Common::getCharacter("character_9/save_1/data.sav");
    // for (int i=0; i<character.inventory.size(); i++) {
    //     QJsonArray item = character.inventory.at(i).toArray();
    //     QJsonObject itemInfo = item.at(1).toObject();
    //     if (itemInfo.value("idName") == "tinker") {
    //         qDebug()<< item;
    //     }
    //     qDebug()<<itemInfo;
    // }
    for (QString key : character.character.keys()) {
        if (key != "Books_Read") {
            // continue;
        }
        // qDebug()<< key << character.character.value(key);
    }
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
    Speed::stop();
}

void MainWindow::init()
{
    QString saveDir = Common::getSaveDir();
    ui->lineEdit->setText(saveDir);
    ui->lineEdit->setReadOnly(true);
    if (QFile::exists(saveDir)) {
        ui->lineEdit->setStyleSheet("QLineEdit { background-color:rgba(0, 255, 0, 0.1);}");
        ui->label_3->setText("已启用（关闭程序会失效）");
        Common::automaticBackup();
    } else {
        ui->lineEdit->setStyleSheet("QLineEdit { background-color:rgba(255, 0, 0, 0.1);}");
    }
    ui->label_7->setPixmap(QPixmap(":/icon/icon.png").scaled(60,80,Qt::KeepAspectRatio));
    m_getSpeedTimer = new QTimer(this);
    m_getSpeedTimer->setInterval(1000);
    connect(m_getSpeedTimer,&QTimer::timeout,this,&MainWindow::on_getSpeedTimerTimeout);
    m_updater = new Updater(this);
    connect(m_updater,&Updater::updateMsg,this,&MainWindow::updateMsg);
    setWindowTitle("stoneshard helper");
    setWindowIcon(QIcon(":/icon/icon.png"));
    // setStyleSheet("QFrame {background-color: rgba(255, 255, 255, 0.5);} QMainWindow{background-color: rgb(28, 28, 52);}");
    ui->pushButton_2->setEnabled(false);
    ui->statusbar->showMessage("程序完全免费，无论从任何渠道购买，速速申请退款");
    ui->statusbar->setStyleSheet("QStatusBar{ color: red; font-weight: bold;}");
    ui->label->setText("剩余点数:"+QString::number(m_pointMax));
    ui->comboBox_2->addItems(Common::filterType);
    on_pushButton_clicked();
    QAction *act;
    act = menuBar()->addAction("",[]{
        QDesktopServices::openUrl(QUrl("https://space.bilibili.com/107071365"));
    });
    act->setIcon(QIcon(":/icon/bilibili.png"));
    act = menuBar()->addAction("",[]{
        QDesktopServices::openUrl(QUrl("https://github.com/YouYuFei/stoneshard-helper"));
    });
    act->setIcon(QIcon(":/icon/github.png"));
    act = menuBar()->addAction("",[]{
        QDesktopServices::openUrl(QUrl("https://yyf.luxe/stoneshard-helper/support"));
    });
    act->setIcon(QIcon(":/icon/support.png"));
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
        item->setWhatsThis(Common::filterType.at(sup.filterType));
        ui->tableWidget->setItem(i, 0, item);
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(sup.description));
        QString point = QString::number(sup.point);
        if (!point.startsWith('-')) {
            point = "+" + point;
        }
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(point));
    }
    on_comboBox_currentTextChanged(ui->comboBox->currentText());
    QList<QWidget*> widgets = this->findChildren<QWidget*>();
    foreach(QWidget* widget, widgets) {
        widget->setFocusPolicy(Qt::NoFocus);
    }
    m_getSpeedTimer->start();
}

void MainWindow::on_pushButton_clicked()
{
    ui->comboBox->clear();
    bool hasNewSave = false;
    QList<CharacterData> list = Common::getNewCharacterList();
    QStandardItemModel *model = new QStandardItemModel(ui->comboBox);
    for (CharacterData data : list) {
        QStandardItem *item = new QStandardItem();
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
    ui->statusbar->showMessage("操作完成");
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
    ui->label_7->setPixmap(QPixmap(data.icon).scaled(60,80,Qt::KeepAspectRatio));
    for (int i=0; i<ui->tableWidget->rowCount(); i++) {
        QTableWidgetItem *item0 = ui->tableWidget->item(i,0);
        QTableWidgetItem *item1 = ui->tableWidget->item(i,1);
        QTableWidgetItem *item2 = ui->tableWidget->item(i,2);
        item0->setSelected(false);
        item1->setSelected(false);
        item2->setSelected(false);
        InitialSupply sup = item0->data(Qt::UserRole + 1).value<InitialSupply>();
        if (sup.characterName.isEmpty()) {
            continue;
        }
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

void MainWindow::on_comboBox_2_currentTextChanged(const QString &arg1)
{
    for (int i=0; i<ui->tableWidget->rowCount(); i++) {
        QTableWidgetItem* item = ui->tableWidget->item(i,0);
        if (arg1 == Common::filterType.first()) {
           ui->tableWidget->setRowHidden(i, false);
        } else if (arg1 != item->whatsThis()) {
            ui->tableWidget->setRowHidden(i, true);
        } else {
            ui->tableWidget->setRowHidden(i, false);
        }
    }
}

void MainWindow::on_getSpeedTimerTimeout()
{
    if (Speed::GetStatus()) {
        ui->horizontalSlider->setEnabled(true);
        m_getSpeedTimer->stop();
    }
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    double speed = double(value) / double(10);
    ui->label_8->setText(QString::number(speed, 'f', 1));
    if (!Speed::setSpeed(speed)) {
        m_getSpeedTimer->start();
        ui->horizontalSlider->setEnabled(false);
    }
}

