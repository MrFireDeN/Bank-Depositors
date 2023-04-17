#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "deposit.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ФИО
    QValidator *fio = new FIOValidator(this);
    ui->fullnameLine->setValidator(fio);

    // Дата рождения
    ui->BirthdayDate->setDateRange(QDate::currentDate().addYears(-100), QDate::currentDate().addYears(-18));

    // Сумма вклада
    ui->depositAmountNumber->setMinimum(0.0); //всегда больше 0
    //Если сумма вклада равна нулю сделать окна "Процент по вкладу" и "Переодичность начисления" неактивными
    if (ui->depositAmountNumber->value() <= 0){
        ui->depositInterest->setEnabled(false);
        ui->accrualFrequency->setEnabled(false);
    }
    else {
        ui->depositInterest->setEnabled(true);
        ui->accrualFrequency->setEnabled(true);
    }

    // Дата последней операции по вкладу
    ui->lastDepositTransactionDate->setDateRange(QDate::currentDate().addYears(-17), QDate::currentDate());

    // Создание депозитов
    Deposit deposit;
    deposits.append(deposit);
    deposits.append(deposit);

    // Тип депозита
    depositType.append(ui->termDeposit); // срочный
    depositType.append(ui->demandDeposit); // до востребования
    depositType.append(ui->irreducibleDeposit); //с неснижаемым остатком

    // Переодчность начисления
    accrualFrequency.append(ui->weekly); // еженедельно
    accrualFrequency.append(ui->monthly); // ежемесячно
    accrualFrequency.append(ui->quarterly); // ежеквартально
    accrualFrequency.append(ui->annually); // ежегодно

    // Вывод интерфейса
    ui->buttonRecord1->setEnabled(false);
    showDeposit(deposits[recordType]);
    on_fullnameLine_textEdited();
    on_accountNumberLine_textEdited();
}

MainWindow::~MainWindow()
{
    delete ui;
}


// Сохранение записи депозита
void MainWindow::saveDeposit(Deposit& deposit) {
    if (!ui->fullnameLine->hasAcceptableInput() || ui->accountNumberLine->text().length() != 20)
        return;

    deposit.accountNumber = ui->accountNumberLine->text();
    deposit.amount = ui->depositAmountNumber->value();
    deposit.interest = ui->depositInterestNumber->value();
    deposit.FIO = ui->fullnameLine->text();
    deposit.birthDate = ui->BirthdayDate->date();
    deposit.lastTransaction = ui->lastDepositTransactionDate->date();
    deposit.plasticCardAvailability = ui->plasticCardAvailability->isChecked();
    deposit.type = whichRadioButtonChecked(depositType);
    deposit.accrualFrequency = whichRadioButtonChecked(accrualFrequency);
};

// Показать запись депозита
void MainWindow::showDeposit(Deposit& deposit) {
    ui->accountNumberLine->setText(deposit.accountNumber);
    ui->depositAmountNumber->setValue(deposit.amount);
    ui->depositInterestNumber->setValue(deposit.interest);
    ui->fullnameLine->setText(deposit.FIO);
    ui->BirthdayDate->setDate(deposit.birthDate);
    ui->lastDepositTransactionDate->setDate(deposit.lastTransaction);
    ui->plasticCardAvailability->setChecked(deposit.plasticCardAvailability);
    depositType[deposit.type]->setChecked(true);
    accrualFrequency[deposit.accrualFrequency]->setChecked(true);
    on_accountNumberLine_textEdited();
    on_fullnameLine_textEdited();
}

// Смена записи депозита
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

// Поиск выбранного элемента в QGroupBox
int MainWindow::whichRadioButtonChecked(QList <QRadioButton*> rd) {
    for (int i = 0; i < rd.length(); i++) {
        if(rd[i]->isChecked())
            return i;
    }
    return 0;
}

// Если сумму депозита изменили
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

// Нажата запись1
void MainWindow::on_buttonRecord1_clicked()
{
    changeDeposit(recordType);
}

// Нажата запись2
void MainWindow::on_buttonRecord2_clicked()
{
    changeDeposit(recordType);
}

// Нажата кнопка показать
void MainWindow::on_buttonRecordShow_clicked()
{
    showDeposit(deposits[recordType]);
}

// Нажата кнопка сохранить
void MainWindow::on_buttonRecordSave_clicked()
{
    saveDeposit(deposits[recordType]);
}

// Проверка введенного ФИО на корректность
void MainWindow::on_fullnameLine_textEdited()
{
    if(ui->fullnameLine->hasAcceptableInput())
        ui->fullnameError->hide();
    else
        ui->fullnameError->show();
}

// Проверка введенного номера счета на корректность
void MainWindow::on_accountNumberLine_textEdited()
{
    if (ui->accountNumberLine->text().length() == 20)
        ui->accountNumberError->hide();
    else
        ui->accountNumberError->show();
}
