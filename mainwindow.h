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
#include <QFileDialog>>
#include <QMessageBox>

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
    QString Filename;

private slots:
    void saveDeposit(Deposit&);
    void showDeposit(Deposit&);
    int whichRadioButtonChecked(QList <QRadioButton*>);
    void setUIEnabled(bool);
    void addRow(Deposit&);

    void on_depositAmountNumber_valueChanged(double);
    void on_recordSave_clicked();
    void on_fullnameLine_textEdited();
    void on_accountNumberLine_textEdited();
    void on_recordCreate_clicked();
    void on_recordBrowserTable_cellClicked(int row, int column);
    void on_recordDelete_clicked();
    void on_recordBrowserButton_clicked();

    void on_saveFileButon_clicked();


    void on_openFileButton_clicked();

    void on_saveAsFileButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
