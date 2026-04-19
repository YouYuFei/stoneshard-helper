#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "common.h"
#include "speed.h"
#include "steam.h"
#include <QStandardItemModel>
#include <QDesktopServices>
#include <QMouseEvent>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
#ifndef QT_NO_DEBUG
    //仅调试用
    m_pointMax += 90;
    // CharacterData character = Common::getCharacter("character_3/save_1/data.sav");
    // for (int i=0; i<character.inventory.size(); i++) {
    //     QJsonArray item = character.inventory.at(i).toArray();
    //     QJsonObject itemInfo = item.at(1).toObject();
    //     // if (itemInfo.value("idName") == "tinker") {
    //     //     qDebug()<< item;
    //     // }
    //     qDebug()<<itemInfo;
    // }
    // for (QString key : character.character.keys()) {
    //     if (key != "buffs") {
    //         continue;
    //     }
    //     qDebug()<< key << character.character.value(key);
    // }
    // QJsonObject root = character.origData;
    // QJsonDocument newDoc(root);
    // QByteArray newJson = newDoc.toJson(QJsonDocument::Indented);
    // Common::fastWrite("save_2.json",newJson);
#else
    centerWindow();
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
    setWindowTitle("stoneshard helper");
    setWindowIcon(QIcon(":/icon/icon.png"));
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    QWidget* titleBar = new QWidget(this);
    titleBar->setObjectName("titleBar");
    titleBar->installEventFilter(this);
    titleBar->setFixedHeight(40);
    QHBoxLayout *layout = new QHBoxLayout(titleBar);
    QLabel *titleIcon = new QLabel(this);
    titleIcon->setPixmap(windowIcon().pixmap(26,26));
    layout->addWidget(titleIcon);
    layout->addWidget(new QLabel(this->windowTitle()));
    layout->addStretch();
    QPushButton *bilibili = new QPushButton(QIcon(":/icon/bilibili.png"),"",this);
    QPushButton *github = new QPushButton(QIcon(":/icon/github.png"),"",this);
    QPushButton *support = new QPushButton(QIcon(":/icon/support.png"),"",this);
    QPushButton *min = new QPushButton(getInvertedIcon(QStyle::SP_TitleBarMinButton),"",this);
    QPushButton *max = new QPushButton(getInvertedIcon(QStyle::SP_TitleBarMaxButton),"",this);
    QPushButton *close = new QPushButton(getInvertedIcon(QStyle::SP_TitleBarCloseButton),"",this);
    for (auto btn : {bilibili,github,support}) {
        btn->setFixedSize(26,26);
        layout->addWidget(btn);
    }
    QFrame *verticalLine = new QFrame;
    verticalLine->setFrameShape(QFrame::VLine);
    verticalLine->setMinimumHeight(26);
    layout->addWidget(verticalLine);
    for (auto btn : {min,max,close}) {
        btn->setFixedSize(26,26);
        layout->addWidget(btn);
    }
    setMenuWidget(titleBar);
    connect(bilibili,&QPushButton::clicked,this,[&]{QDesktopServices::openUrl(QUrl("https://space.bilibili.com/107071365"));});
    connect(github,&QPushButton::clicked,this,[&]{QDesktopServices::openUrl(QUrl("https://github.com/YouYuFei/stoneshard-helper"));});
    connect(support,&QPushButton::clicked,this,[&]{QDesktopServices::openUrl(QUrl("https://yyf.luxe/stoneshard-helper/support"));});
    connect(min, &QPushButton::clicked, this, &MainWindow::showMinimized);
    connect(max, &QPushButton::clicked, [=] {
        if (isMaximized()) {
            max->setIcon(getInvertedIcon(QStyle::SP_TitleBarMaxButton));
            showNormal();
        } else {
            max->setIcon(getInvertedIcon(QStyle::SP_TitleBarNormalButton));
            showMaximized();
        }
    });
    connect(close, &QPushButton::clicked, qApp, &QApplication::quit);
    m_achievements = Steam::getUnlockedAchievements();
    m_playTime = Steam::getPlaytime();
    int pointAchievements = m_achievements / 2;
    int pointPlayTime = m_playTime / 60 / 100;
    QString pointFrom = QString("总点数:%1 (基础:%2 + 成就加成:%3 + 游玩时间加成:%4)")
            .arg(m_pointMax + pointAchievements + pointPlayTime)
            .arg(m_pointMax)
            .arg(pointAchievements)
            .arg(pointPlayTime);
    ui->label_9->setText(pointFrom);
    ui->label_9->setToolTip(QString("steam已解锁%1个成就(每解锁2个成就加1配置点)，steam游玩时长%2分钟(每游玩100小时加1配置点)").arg(m_achievements).arg(m_playTime));
    QString saveDir = Common::getSaveDir();
    ui->lineEdit->setText(saveDir);
    ui->lineEdit->setReadOnly(true);
    if (QFile::exists(saveDir)) {
        ui->label_3->setText("已启用（关闭程序会失效）");
        Common::automaticBackup();
    }
    m_getSpeedTimer = new QTimer(this);
    m_getSpeedTimer->setInterval(DEFAULT_TIMEOUT);
    connect(m_getSpeedTimer,&QTimer::timeout,this,&MainWindow::on_getSpeedTimerTimeout);
    m_updater = new Updater(this);
    connect(m_updater,&Updater::updateMsg,this,&MainWindow::updateMsg);
    ui->pushButton_2->setEnabled(false);
    ui->statusbar->showMessage("程序完全免费，无论从任何渠道购买，速速申请退款");
    ui->label->setText("可用配置点:"+QString::number(m_pointMax + pointAchievements + pointPlayTime));
    ui->comboBox_2->addItems(Common::filterType);
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
        item->setWhatsThis(sup.filterType);
        ui->tableWidget->setItem(i, 0, item);
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(sup.description));
        QString point = QString::number(sup.point);
        if (!point.startsWith('-')) {
            point = "+" + point;
        }
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(point));
    }
    on_pushButton_clicked();
    on_comboBox_currentTextChanged(ui->comboBox->currentText());
    m_getSpeedTimer->start();
    QList<QWidget*> widgets = this->findChildren<QWidget*>();
    foreach(QWidget* widget, widgets) {
        widget->setFocusPolicy(Qt::NoFocus);
    }
    setStyleSheet("QMainWindow{background-color:transparent;}"
                  "*{color:#fff;}"
                  "*:disabled{color:#888;}");
    ui->centralwidget->setStyleSheet("*{background-color:rgb(28,28,52);}"
                                     "QFrame{border-image: url(:/ui/Small_border.png) 24 stretch;border-width:24px;padding:-14px;}"
                                     "QLabel,QLineEdit{border:none;padding:0px;}"
                                     "QComboBox,QPushButton{padding: 5px 20px;background-color:rgb(50,50,90);}"
                                     "QPushButton:hover,QComboBox:hover{background-color:rgb(100,100,150);}"
                                     "QComboBox QAbstractItemView{border:none;}");
    ui->comboBox->view()->window()->setStyleSheet("background-color:rgb(28,28,52);border:17px solid;");
    ui->comboBox_2->view()->window()->setStyleSheet("background-color:rgb(28,28,52);border:17px solid;");
    verticalLine->setStyleSheet("background-color:#fff;margin:2px;");
    titleBar->setStyleSheet("QWidget{background-color:rgb(28,28,52);border-top-left-radius: 15px;border-top-right-radius:15px;}"
                            "QPushButton{padding: 5px 20px;background-color:rgb(50,50,90);border-radius:5px}"
                            "QPushButton:hover{background-color:rgb(100,100,150);}");
    ui->statusbar->setStyleSheet("QStatusBar{background-color:rgb(28,28,52);color:red;font-weight:bold;border-bottom-left-radius: 15px;border-bottom-right-radius: 15px;}");
    ui->tableWidget->setStyleSheet("QTableWidget{background-color:rgb(28,28,52);border-width:20px;}"
                                   "QHeaderView{border:none;padding:0px;}"
                                   "QTableWidget::item:hover{background-color:rgb(50,50,90);}"
                                   "QTableWidget::item:selected{background-color:rgb(100,100,150);}");
    ui->label_7->setStyleSheet("border:5px solid;");
    ui->label->setStyleSheet("font-weight:bold;");
    ui->statusbar->setFixedHeight(40);
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
    int pointAchievements = m_achievements / 2;
    int pointPlayTime = m_playTime / 60 / 100;
    int remaining = m_pointMax + pointAchievements + pointPlayTime - usePoint;
    ui->label->setText("可用配置点:"+QString::number(remaining));
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
    QString iconUrl = data.icon;
    if (iconUrl.isEmpty()) {
        iconUrl = ":/portrait/32px-Frontpage_POIs.png";
    }
    ui->label_7->setPixmap(QPixmap(iconUrl).scaled(60,80));
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

bool MainWindow::eventFilter(QObject *obj, QEvent *e)
{
    if (obj->objectName() == "titleBar") {
            if (e->type() == QEvent::MouseButtonPress) {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(e);
                if (mouseEvent->button() == Qt::LeftButton) {
                    m_isDragging = true;
                    m_dragStartPosition = mouseEvent->globalPos() - frameGeometry().topLeft();
                }
            }
            else if (e->type() == QEvent::MouseMove && m_isDragging) {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(e);
                if (mouseEvent->buttons() & Qt::LeftButton) {
                    move(mouseEvent->globalPos() - m_dragStartPosition);
                }
            }
            else if (e->type() == QEvent::MouseButtonRelease) {
                m_isDragging = false;
            }
        }
    return obj->event(e);
}

QIcon MainWindow::getInvertedIcon(QStyle::StandardPixmap standardPixmap) {
    QIcon icon = style()->standardIcon(standardPixmap);
    QPixmap pixmap = icon.pixmap(16, 16);
    QImage image = pixmap.toImage();
    image.invertPixels(QImage::InvertRgb);
    return QIcon(QPixmap::fromImage(image));
}

void MainWindow::centerWindow()
{
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    move(screenGeometry.center() - rect().center());
}

