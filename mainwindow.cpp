#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "deposit.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //ФИО
    QValidator *fio = new FIOValidator(this);
    ui->fullnameLine->setValidator(fio);

    //Дата рождения
    ui->BirthdayDate->setDateRange(QDate::currentDate().addYears(-100), QDate::currentDate().addYears(-18));

    //Сумма вклада
    ui->depositAmountNumber->setMinimum(0.0); //всегда больше 0//Если сумма вклада равна нулю сделать окна "Процент по вкладу" и "Переодичность начисления" неактивными
    if (ui->depositAmountNumber->value() <= 0){
        ui->depositInterest->setEnabled(false);
        ui->accrualFrequency->setEnabled(false);
    }
    else {
        ui->depositInterest->setEnabled(true);
        ui->accrualFrequency->setEnabled(true);
    }

    //Дата последней операции по вкладу
    ui->lastDepositTransactionDate->setDateRange(QDate::currentDate().addYears(-17), QDate::currentDate());

    //Создание депозитов
    Deposit deposit;
    deposits.append(deposit);
    deposits.append(deposit);

    ui->buttonRecord2->setEnabled(false);
    showDeposit(deposits[recordType]);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_depositAmountNumber_valueChanged(double value)
{
    //Если сумма вклада равна нулю сделать окна "Процент по вкладу" и "Переодичность начисления" неактивными
    if (value <= 0){
        ui->depositInterest->setEnabled(false);
        ui->accrualFrequency->setEnabled(false);
    }
    else {
        ui->depositInterest->setEnabled(true);
        ui->accrualFrequency->setEnabled(true);
    }
    ui->lastDepositTransactionDate->setDate(QDate::currentDate());
}

void MainWindow::saveDeposit(Deposit& deposit) {
    deposit.accountNumber = ui->accountNumberLine->text();
    deposit.amount = ui->depositAmountNumber->value();
    deposit.interest = ui->depositInterestNumber->value();
    //deposit.accrualFrequency.setTitle(ui->accrualFrequency->title());
    deposit.FIO = ui->fullnameLine->text();
    deposit.birthDate = ui->BirthdayDate->date();
    //deposit.Type.setTitle(ui->depositType->title());
    deposit.lastTransaction = ui->lastDepositTransactionDate->date();
    deposit.plasticCardAvailability = ui->plasticCardAvailability;
};

void MainWindow::showDeposit(Deposit& deposit) {
    ui->accountNumberLine->setText(deposit.accountNumber);
    ui->depositAmountNumber->setValue(deposit.amount);
    ui->depositInterestNumber->setValue(deposit.interest);
    //ui->accrualFrequency->;
    ui->fullnameLine->setText(deposit.FIO);
    ui->BirthdayDate->setDate(deposit.birthDate);
    ui->depositType->setTitle(deposit.typeTitle);
    ui->lastDepositTransactionDate->setDate(deposit.lastTransaction);
    ui->plasticCardAvailability->setTristate(deposit.plasticCardAvailability);
}

void MainWindow::changeDeposit(bool &recordType) {
    recordType = !recordType;
    if(recordType) {
        ui->buttonRecord1->setEnabled(true);
        ui->buttonRecord2->setEnabled(false);
    }
    else {
        ui->buttonRecord2->setEnabled(true);
        ui->buttonRecord1->setEnabled(false);
    }
}

void MainWindow::on_buttonRecord1_clicked()
{
    changeDeposit(recordType);
}

void MainWindow::on_buttonRecord2_clicked()
{
    changeDeposit(recordType);
}

void MainWindow::on_buttonRecordShow_clicked()
{
    showDeposit(deposits[recordType]);
}

void MainWindow::on_buttonRecordSave_clicked()
{
    saveDeposit(deposits[recordType]);
}
