#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <fiovalidator.h>
#include <deposit.h>
#include <QRadioButton>
#include <QTableWidget>
#include "depositDatabase.h"
#include "randomrecord.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QVector <Deposit> deposits;
    int recordType = 0;
    QList <QRadioButton*> depositType;
    QList <QRadioButton*> accrualFrequency;
    DepositDatabase dd;
    Deposit deposit;
    QString Filename = "main.dd";

private slots:
    void updateFiles();

    void saveDeposit(Deposit&);
    void showDeposit(Deposit&);

    void createRecord();
    void saveRecord();
    void removeRecord();
    void createRandomRecords();

    void selectRecord(int);
    void addRow(Deposit&, int);
    void showRecords();

    void openFile();
    void saveFile();

    void setUIEnabled(bool);
    int whichRadioButtonChecked(QList <QRadioButton*>);
    void checkDepositAmount(double);
    void checkFIO();
    void checkAccountNumber();

private:
    Ui::MainWindow *ui;

    QTimer *timer;
};

#endif // MAINWINDOW_H
