#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->recordCreate, SIGNAL(clicked(bool)), this, SLOT(createRecord()));
    connect(ui->recordSave, SIGNAL(clicked(bool)), this, SLOT(saveRecord()));
    connect(ui->recordDelete, SIGNAL(clicked(bool)), this, SLOT(removeRecord()));
    connect(ui->recordBrowserButton, SIGNAL(clicked(bool)), this, SLOT(createRandomRecords()));

    connect(ui->fullnameLine, SIGNAL(textChanged(QString)), this, SLOT(checkFIO));
    connect(ui->depositAmountNumber, SIGNAL(valueChanged(double)), this, SLOT(checkDepositAmount(double)));
    connect(ui->accountNumberLine, SIGNAL(textChanged(QString)), this, SLOT(checkAccountNumber()));
    connect(ui->recordBrowserTable, SIGNAL(cellClicked(int,int)), this, SLOT(selectRecord(int)));

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
    checkFIO();
    checkAccountNumber();

    srand(time(0));

    // Подключение к базе данных
    dd.connect();

    // Открытие из базы данных
    openFile();

    // Создайте и настройте таймер
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateFiles); // Подключите слот updateFiles к сигналу timeout
    timer->start(15000); // Запустите таймер с интервалом в 15 секунд
}

// Функция для обновления файлов
void MainWindow::updateFiles()
{
    saveDeposit(deposit);
    Deposit currentRecord = deposit;

    openFile();

    deposit = currentRecord;
    showDeposit(deposit);

    qDebug() << "Data changed";
}

MainWindow::~MainWindow()
{
    // Остановите таймер и освободите ресурсы
    timer->stop();
    delete timer;

    // Сохрание в базу данных
    saveFile();

    // Отключенние от базы данных
    dd.disconnect();

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
    checkAccountNumber();
    checkFIO();
}

// Создать запись
void MainWindow::createRecord()
{
    deposit = *(new Deposit); // задаёт значение записи по умолчанию

    recordType = dd.append(deposit); // добавляет запись в базу данных

    showRecords();
    showDeposit(deposit); // выводит брауез в виджеты
}

// Сохранить запись
void MainWindow::saveRecord()
{
    saveDeposit(deposit); // сохраняет значение из виджетов в запись из браузера
    recordType = dd.update(deposit); // обновляет запись в базе данных

    for (int i = 0; i < ui->recordBrowserTable->rowCount(); ++i) {
        qDebug() << ui->recordBrowserTable->item(i, 0)->data(Qt::UserRole).toInt();
    }

    showRecords();
}

// Удалить запись
void MainWindow::removeRecord()
{
    dd.remove(deposit.id); // удаляет запись из базы данных

    // фокус на предыдущию запись если, прошлая была последней
    if (recordType == dd.count() && recordType > 0)
        recordType--;

    showRecords();
}

// Добавить 10 случайных записей
void MainWindow::createRandomRecords()
{
    RandomRecord randomRecord;

    for (int i = 0; i < 10; i++) {
        randomRecord.setRecord(deposit);
        recordType = dd.append(deposit);
    }

    showRecords();
    showDeposit(deposit);
}

// Выбрать запись из браузера
void MainWindow::selectRecord(int row)
{
    recordType = row; // присваивание строки

    // нахождение элемента по id
    unsigned int id = ui->recordBrowserTable->item(recordType, 0)->data(Qt::UserRole).toInt();
    dd.record(id, deposit);
    showDeposit(deposit);

    ui->recordBrowserTable->selectRow(recordType); // фокус на строку
}

// Добавить строку в браузер
void MainWindow::addRow(Deposit &deposit, int row) {
    ui->recordBrowserTable->insertRow(row); // вставка новой строки

    QTableWidgetItem *item; // создание элемента

    // заполнение строки
    item = new QTableWidgetItem(deposit.accountNumber);
    item->setData(Qt::UserRole, deposit.id);

    ui->recordBrowserTable->setItem(row, 0, item);
    QString amount;
    int rub = (int) deposit.amount;
    int kop = (int) ((deposit.amount - rub) * 100 + 0.1);
    amount = QString("%1 руб. %2 коп.").arg(rub).arg(kop);
    item = new QTableWidgetItem(amount);

    ui->recordBrowserTable->setItem(row, 1, item);
}

void MainWindow::showRecords() {
    ui->recordBrowserTable->clear();
    ui->recordBrowserTable->setRowCount(0);

    QVector<DepositDatabase::RecordRow> rr = dd.records();

    for (int i = 0; i < rr.count(); i++) {
        deposit.id = rr[i].id;
        deposit.accountNumber = rr[i].accountNumber;
        deposit.amount = rr[i].amount;
        addRow(deposit, i);
    }

    if(dd.count() > 0) {
        setUIEnabled(true);
        selectRecord(recordType);
        ui->recordBrowserTable->selectRow(recordType); // выделение строки
    }
    else {
        setUIEnabled(false);
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
    if (!dd.load()) {
        QMessageBox::information(nullptr, "Ошибка", "Файла не существует");
    }
    showRecords();
}

// Сохранить файл базы данных
void MainWindow::saveFile() {
    if (!dd.save())
        QMessageBox::information(nullptr, "Ошибка", "Файл не сохранен");
}

// Проверка суммы вклада
void MainWindow::checkDepositAmount(double value)
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
void MainWindow::checkFIO()
{
    if(ui->fullnameLine->hasAcceptableInput())
        ui->fullnameError->hide();
    else
        ui->fullnameError->show();
}

// Проверка введенного номера счета на корректность
void MainWindow::checkAccountNumber()
{
    if (ui->accountNumberLine->text().length() == 20)
        ui->accountNumberError->hide();
    else
        ui->accountNumberError->show();
}
