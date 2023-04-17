#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <fiovalidator.h>
#include <deposit.h>
#include <QRadioButton>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QList <Deposit> deposits;
    bool recordType = 0;
    QList <QRadioButton*> depositType;
    QList <QRadioButton*> accrualFrequency;


private slots:
    void on_depositAmountNumber_valueChanged(double arg1);
    void saveDeposit(Deposit&);
    void showDeposit(Deposit&);
    void changeDeposit(bool&);

    void on_buttonRecord1_clicked();

    void on_buttonRecord2_clicked();

    void on_buttonRecordShow_clicked();

    void on_buttonRecordSave_clicked();

    int whichRadioButtonChecked(QList <QRadioButton*>);

    void on_fullnameLine_textEdited();

    void on_accountNumberLine_textEdited();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
