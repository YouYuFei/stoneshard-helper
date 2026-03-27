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
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    for (int i=0; i<list.size(); i++ ) {
        InitialSupply sup = list.at(i);
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(sup.nameKey));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(sup.description));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(sup.point)));
    }
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
            item->setEnabled(false);
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
    QList<bool> list;
    for (int i=0; i<ui->tableWidget->rowCount(); i++) {
        QTableWidgetItem* item = ui->tableWidget->item(i,0);
        list.append(item->isSelected());
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
            usePoint += ui->tableWidget->item(i,2)->text().toInt();
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

void MainWindow::on_comboBox_currentIndexChanged(int)
{
    for (int i=0; i<ui->tableWidget->rowCount(); i++) {
        ui->tableWidget->item(i,0)->setSelected(false);
        ui->tableWidget->item(i,1)->setSelected(false);
        ui->tableWidget->item(i,2)->setSelected(false);
    }
}

void MainWindow::updateMsg(QString msg, QString note)
{
    ui->statusbar->showMessage(msg);
    ui->statusbar->setToolTip(note);
}
