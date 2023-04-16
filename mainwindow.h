#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <fiovalidator.h>
#include <deposit.h>

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

private slots:
    void on_depositAmountNumber_valueChanged(double arg1);
    void saveDeposit(Deposit&);
    void showDeposit(Deposit&);
    void changeDeposit(bool&);

    void on_buttonRecord1_clicked();

    void on_buttonRecord2_clicked();

    void on_buttonRecordShow_clicked();

    void on_buttonRecordSave_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
