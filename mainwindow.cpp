#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "deposit.h"
#include "randomrecord.h"
#include <algorithm>

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

    // Создание колонок
    QStringList headerLabels;
    headerLabels << "Номер лицевого счета" << "Сумма вклада";
    ui->recordBrowserTable->setColumnCount(headerLabels.count());
    ui->recordBrowserTable->setHorizontalHeaderLabels(headerLabels);
    ui->recordBrowserTable->setColumnWidth(0, 200);
    ui->recordBrowserTable->setColumnWidth(1, 90);


    // Создание депозитов
    Deposit deposit;
    //deposits.append(deposit);
    //deposits.append(deposit);

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
    //ui->buttonRecord1->setEnabled(false);
    showDeposit(deposit);
    on_fullnameLine_textEdited();
    on_accountNumberLine_textEdited();

    setUIEnabled(false);
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

// Поиск выбранного элемента в QGroupBox
int MainWindow::whichRadioButtonChecked(QList <QRadioButton*> rd) {
    for (int i = 0; i < rd.length(); i++) {
        if(rd[i]->isChecked())
            return i;
    }
    return 0;
}

// Видимость полей редактирования
void MainWindow::setUIEnabled(bool x){
    ui->accountNumber->setEnabled(x);
    ui->accrualFrequency->setEnabled(x);
    ui->depositAmount->setEnabled(x);
    ui->depositInterest->setEnabled(x);
    ui->depositType->setEnabled(x);
    ui->lastDepositTransaction->setEnabled(x);
    ui->passportData->setEnabled(x);
    ui->plasticCardAvailability->setEnabled(x);
    ui->recordSave->setEnabled(x);
    ui->recordDelete->setEnabled(x);
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


// нажата кнопка создать
void MainWindow::on_recordCreate_clicked()
{

    Deposit deposit;
    deposits.append(deposit);

    ui->recordBrowserTable->setRowCount(deposits.count());

    QTableWidgetItem *item;

    item = new QTableWidgetItem(deposit.accountNumber);
    ui->recordBrowserTable->setItem(deposits.count()-1, 0, item);
    QString amount;
    int rub = (int) deposit.amount;
    int kop = (int) ((deposit.amount - rub) * 100 + 0.1);
    amount = QString("%1 руб. %2 коп.").arg(rub).arg(kop);
    item = new QTableWidgetItem(amount);
    ui->recordBrowserTable->setItem(deposits.count()-1, 1, item);

    if(deposits.count() == 1)
        setUIEnabled(true);

    recordType = deposits.count() - 1;
    showDeposit(deposits[recordType]);

    recordSort();
}

// Нажата кнопка сохранить
void MainWindow::on_recordSave_clicked()
{
    saveDeposit(deposits[recordType]);

    QTableWidgetItem *item;

    Deposit deposit = deposits[recordType];

    item = ui->recordBrowserTable->item(recordType, 0);
    *item = QTableWidgetItem(deposit.accountNumber);
    ui->recordBrowserTable->setItem(recordType, 0, item);
    QString amount;
    int rub = (int) deposit.amount;
    int kop = (int) ((deposit.amount - rub) * 100 + 0.1);
    amount = QString("%1 руб. %2 коп.").arg(rub).arg(kop);
                 item = ui->recordBrowserTable->item(recordType, 1);
    *item = QTableWidgetItem(amount);
    ui->recordBrowserTable->setItem(recordType, 1, item);

    showDeposit(deposits[recordType]);
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


void MainWindow::on_recordBrowserTable_cellClicked(int row, int column)
{
    qDebug() << "Фокус на ячейке (" << row << ", " << column << ")";
    recordType = row;
    showDeposit(deposits[recordType]);
}


void MainWindow::on_recordDelete_clicked()
{
    QTableWidgetItem *item;
    item = ui->recordBrowserTable->item(recordType, 0);
    delete item;
    item = ui->recordBrowserTable->item(recordType, 1);
    delete item;
    deposits.remove(recordType);
    ui->recordBrowserTable->removeRow(recordType);

    if (deposits.count() == 0){
        setUIEnabled(false);
        return;
    }

    if (recordType < deposits.count())
        showDeposit(deposits[recordType]);
    else
        showDeposit(deposits[--recordType]);
}


void MainWindow::on_recordBrowserButton_clicked()
{
    RandomRecord rr;
    srand(time(0));

    for (int i = 0; i < 10; i++) {
        Deposit deposit;
        rr.setRecord(deposit);
        deposits.append(deposit);

        ui->recordBrowserTable->setRowCount(deposits.count());

        QTableWidgetItem *item;

        item = new QTableWidgetItem(deposit.accountNumber);
        ui->recordBrowserTable->setItem(deposits.count()-1, 0, item);
        QString amount;
        int rub = (int) deposit.amount;
        int kop = (int) ((deposit.amount - rub) * 100 + 0.1);
        amount = QString("%1 руб. %2 коп.").arg(rub).arg(kop);
                     item = new QTableWidgetItem(amount);
        ui->recordBrowserTable->setItem(deposits.count()-1, 1, item);

        if(deposits.count() == 1)
            setUIEnabled(true);
    }

    recordType = deposits.count() - 1;
    showDeposit(deposits[recordType]);

    recordSort();
}

bool MainWindow::compareById(const Deposit& a, const Deposit& b) {
    return a.type < b.type;
}

void MainWindow::recordSort() {
    std::sort(deposits.begin(), deposits.end(), Deposit::depositComparator());
    for (int i = 0; i < deposits.count(); i++) {

        QTableWidgetItem *item;

        item = ui->recordBrowserTable->item(i, 0);
        *item = QTableWidgetItem(deposits[i].accountNumber);
        ui->recordBrowserTable->setItem(i, 0, item);
        QString amount;
        int rub = (int) deposits[i].amount;
        int kop = (int) ((deposits[i].amount - rub) * 100 + 0.1);
        amount = QString("%1 руб. %2 коп.").arg(rub).arg(kop);
        item = ui->recordBrowserTable->item(i, 1);
        *item = QTableWidgetItem(amount);
        ui->recordBrowserTable->setItem(i, 1, item);
    }

    recordType = deposits.count()-1;
    showDeposit(deposits[recordType]);
}
