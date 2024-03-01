#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    on_fullnameLine_textEdited();
    on_accountNumberLine_textEdited();

    setUIEnabled(false);

    srand(time(0));

    openFile();
}

MainWindow::~MainWindow()
{
    saveFile();

    delete ui;
}


// Сохраненить запись из браузера
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

// Показать запись из браузера
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

// Создать запись
void MainWindow::on_recordCreate_clicked()
{
    deposit = *(new Deposit); // задаёт значение записи по умолчанию

    recordType = dd.append(deposit); // добавляет запись в базу данных
    addRow(deposit); // добавляет новую запись в браузер

    // Показывает интерфейс при наличии записей
    if(dd.count() == 1)
        setUIEnabled(true);

    showDeposit(deposit); // выводит брауез в виджеты
}

// Сохранить запись
void MainWindow::on_recordSave_clicked()
{
    saveDeposit(deposit); // сохраняет значение из виджетов в запись
    ui->recordBrowserTable->removeRow(recordType); // удаляет прошлую запись из браузера
    recordType = dd.update(deposit); // обновляет запись в базе данных
    addRow(deposit); // добавляет новую запись в браузер
}

// Удалить запись
void MainWindow::on_recordDelete_clicked()
{
    dd.remove(deposit.id); // удаляет запись из базы данных
    ui->recordBrowserTable->removeRow(recordType); // удаляет строку

    // проверка базы данных на пустоту
    if (dd.count() == 0){
        setUIEnabled(false);
        return;
    }

    // фокус на предыдущию запись если, прошлая была последней
    if (recordType == dd.count())
        recordType--;

    on_recordBrowserTable_cellClicked(recordType, 0); // фокус на запись
}

// Выбрать запись из браузера
void MainWindow::on_recordBrowserTable_cellClicked(int row, int column)
{
    recordType = row; // присваивание строки

    // нахождение элемента по id
    unsigned int id = ui->recordBrowserTable->item(recordType, 0)->data(Qt::UserRole).toInt();
    dd.record(id, deposit);
    showDeposit(deposit);

    ui->recordBrowserTable->selectRow(recordType); // фокус на строку
}

// Добавить 10 случайных записей
void MainWindow::on_recordBrowserButton_clicked()
{
    RandomRecord rr;

    for (int i = 0; i < 10; i++) {
        rr.setRecord(deposit);
        recordType = dd.append(deposit);
        addRow(deposit);

        if(dd.count() == 1)
            setUIEnabled(true);
    }

    // выделить последний элемент браузера
    on_recordBrowserTable_cellClicked(dd.count()-1, 0);
    ui->recordBrowserTable->selectRow(dd.count()-1);
    showDeposit(deposit);
}

// Добавить строку в браузер
void MainWindow::addRow(Deposit &deposit) {
    ui->recordBrowserTable->insertRow(recordType); // вставка новой строки

    QTableWidgetItem *item; // создание элемента

    // заполнение строки
    item = new QTableWidgetItem(deposit.accountNumber);
    ui->recordBrowserTable->setItem(recordType, 0, item);
    QString amount;
    int rub = (int) deposit.amount;
    int kop = (int) ((deposit.amount - rub) * 100 + 0.1);
    amount = QString("%1 руб. %2 коп.").arg(rub).arg(kop);
                 item = new QTableWidgetItem(amount);
    ui->recordBrowserTable->setItem(recordType, 1, item);
    ui->recordBrowserTable->item(recordType, 0)->setData(Qt::UserRole, deposit.id);

    ui->recordBrowserTable->selectRow(recordType); // выделение строки
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

// Сделать полей редактирования (не)активными
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


// Октрывает файл базы данных
void MainWindow::openFile(){
    if (dd.load()) {
        ui->recordBrowserTable->clear();
        ui->recordBrowserTable->setRowCount(0);

        QVector<DepositDatabase::RecordRow> rr = dd.records();

        for (int i = 0; i < rr.count(); i++) {
            deposit.id = rr[i].id;
            deposit.accountNumber = rr[i].accountNumber;
            deposit.amount = rr[i].amount;
            recordType = i;
            addRow(deposit);
        }

        if(dd.count() > 0)
            setUIEnabled(true);

        //recordType = dd.count() -1;
        on_recordBrowserTable_cellClicked(recordType, 0);
    }
    else {
        QMessageBox::information(nullptr, "Ошибка", "Файла не существует");
    }
}

// Сохранить файл базы данных
void MainWindow::saveFile() {
    if (!dd.save())
        QMessageBox::information(nullptr, "Ошибка", "Файл не сохранен");
}

//// Открыть файл базы данных
//void MainWindow::on_openFileButton_clicked()
//{
//    Filename = QFileDialog::getOpenFileName(this,
//                QString("Открыть файл"),
//                QString(),
//                QString("База депозитов (*.dd);;"
//                        "Все файлы (*.*)"));

//    openFile();
//}

//// Сохранить файл базы данных
//void MainWindow::on_saveFileButon_clicked()
//{
//    if (Filename.isEmpty()) {
//        on_saveAsFileButton_clicked();
//        return;
//    }

//    saveFile();
//}


//void MainWindow::on_saveAsFileButton_clicked()
//{
//    Filename = QFileDialog::getSaveFileName(this,
//                                            QString("Сохранить файл"),
//                                            QString(),
//                                            QString("База депозитов (*.dd)"));


//    saveFile();
//}

