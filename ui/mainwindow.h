#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "updater.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_tableWidget_itemSelectionChanged();
    void on_comboBox_currentIndexChanged(int);
    void updateMsg(QString msg, QString note);

private:
    Ui::MainWindow *ui;
    void init();
    Updater *m_updater = nullptr;
    int m_pointMax = 10;
};
#endif // MAINWINDOW_H
