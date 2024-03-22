#include "depositDatabase.h"

DepositDatabase::DepositDatabase()
{

}

// Запустить сервер
bool DepositDatabase::start() {
    // Создание именованного канала
    hPipe = CreateNamedPipe(
        SERVERPIPE,                                         // Имя канала
        PIPE_ACCESS_DUPLEX,                                 // Дуплексный доступ
        PIPE_TYPE_MESSAGE | PIPE_READMODE_BYTE | PIPE_WAIT, // Режимы чтения и ожидания
        PIPE_UNLIMITED_INSTANCES,                           // Количество экземпляров
        1024,                                               // Размер выходного буфера
        1024,                                               // Размер входного буфера
        5000,                                               // Время ожидания по умолчанию (5 секунд)
        NULL                                                // Защита по умолчанию
        );

    if (hPipe == INVALID_HANDLE_VALUE) {
        qDebug() << "Ошибка при создании канала:" << GetLastError();
        return 0;
    }

    qDebug() << "Именованный канал успешно создан.\n";

    // Ожидание соединения
    if (!ConnectNamedPipe(hPipe, NULL)) {
        qDebug() << "Ошибка при ожидании соединения: " << GetLastError();
        CloseHandle(hPipe);
        return 0;
    }

    const DWORD FINISH_REQ = 0, LOAD_REQ = 1, SEND_REQ = 2;

    Deposit rec;
    do {
        //Loading request number
        ReadFile(hPipe, (LPVOID)&req, sizeof(int), &bytesRead, NULL);
        switch (req) {
            case LOAD_REQ:
                load();
                break;
            case SEND_REQ:
                send();
                break;
        }
    } while (req != FINISH_REQ);

    return 1;
}

// Отправить данные на клиент
bool DepositDatabase::send() {
    QVector<Deposit>::iterator i;
    Deposit::D record;

    //Проверка базы дпнных на заполненность
    if (database.empty())
    {
        return false;
    }


    int counts = database.count();

    if (!WriteFile(hPipe, &counts, sizeof(counts), &bytesWritten, NULL))
    {
        return false;
    }

    for (i = database.end()-1; i != database.begin()-1; --i)
    {
        record = Deposit::toStruct(*i);

        // id
        WriteFile(hPipe, (LPVOID)&record.id, sizeof(record.id), &bytesWritten, NULL);
        // Номер счета
        WriteFile(hPipe, (LPVOID)&record.accountNumber, sizeof(record.accountNumber), &bytesWritten, NULL);
        // Тип вклада
        WriteFile(hPipe, (LPVOID)&record.type, sizeof(record.type), &bytesWritten, NULL);
        // ФИО
        WriteFile(hPipe, (LPVOID)&record.FIO, sizeof(record.FIO), &bytesWritten, NULL);
        // Дата рождения
        WriteFile(hPipe, (LPVOID)&record.birthDate, sizeof(record.birthDate), &bytesWritten, NULL);
        // Сумма вклада
        WriteFile(hPipe, (LPVOID)&record.amount, sizeof(record.amount), &bytesWritten, NULL);
        // Процент вклада
        WriteFile(hPipe, (LPVOID)&record.interest, sizeof(record.interest), &bytesWritten, NULL);
        // Переодичность начисления
        WriteFile(hPipe, (LPVOID)&record.accrualFrequency, sizeof(record.accrualFrequency), &bytesWritten, NULL);
        // Последняя транзакция
        WriteFile(hPipe, (LPVOID)&record.lastTransaction, sizeof(record.lastTransaction), &bytesWritten, NULL);
        // Наличие пластиковой карты
        WriteFile(hPipe, (LPVOID)&record.plasticCardAvailability, sizeof(record.plasticCardAvailability), &bytesWritten, NULL);

//        Deposit deposit = Deposit::fromStruct(record);
//        int pos = append(deposit);

//        WriteFile(hPipe, (LPCVOID)&deposit.id, sizeof(unsigned int), &bytesWritten, NULL);
//        WriteFile(hPipe, (LPCVOID)&pos, sizeof(int), &bytesWritten, NULL);
    }

    return 1;

//    // Создание файла
//    HANDLE myFile = CreateFile(
//        FILENAME,               // Имя файла
//        GENERIC_WRITE,          // Желаемый доступ к файлу (здесь только запись)
//        0,                      // Режим разделения (нельзя открывать другим процессам)
//        NULL,                   // Атрибуты безопасности (не используется)
//        CREATE_ALWAYS,          // Режим открытия (открываем существующий файл)
//        FILE_ATTRIBUTE_NORMAL,  // Атрибуты файла (обычный файл)
//        NULL                    // Дескриптор файла-шаблона (не используется)
//        );

//    // Проверка на успешность открытия файла
//    if (myFile == INVALID_HANDLE_VALUE)
//        return false;

//    // Проверка базы дпнных на заполненность
//    if (database.empty()){
//        CloseHandle(myFile);
//        return false;
//    }

//    QVector<Deposit>::iterator i;
//    Deposit::D d;
//    int counts = database.count();

//    WriteFile(myFile, &counts, sizeof(counts), &bytesRead, NULL);


//    for (i = database.end()-1; i != database.begin()-1; --i) {
//        d = Deposit::toStruct(*i);

//        // id
//        WriteFile(myFile, &d.id, sizeof(d.id), &bytesRead, NULL);
//        // Номер счета
//        WriteFile(myFile, &d.accountNumber, sizeof(d.accountNumber), &bytesRead, NULL);
//        // Тип вклада
//        WriteFile(myFile, &d.type, sizeof(d.type), &bytesRead, NULL);
//        // ФИО
//        WriteFile(myFile, &d.FIO, sizeof(d.FIO), &bytesRead, NULL);
//        // Дата рождения
//        WriteFile(myFile, &d.birthDate, sizeof(d.birthDate), &bytesRead, NULL);
//        // Сумма вклада
//        WriteFile(myFile, &d.amount, sizeof(d.amount), &bytesRead, NULL);
//        // Процент вклада
//        WriteFile(myFile, &d.interest, sizeof(d.interest), &bytesRead, NULL);
//        // Переодичность начисления
//        WriteFile(myFile, &d.accrualFrequency, sizeof(d.accrualFrequency), &bytesRead, NULL);
//        // Последняя транзакция
//        WriteFile(myFile, &d.lastTransaction, sizeof(d.lastTransaction), &bytesRead, NULL);
//        // Наличие пластиковой карты
//        WriteFile(myFile, &d.plasticCardAvailability, sizeof(d.plasticCardAvailability), &bytesRead, NULL);
//    }

//    CloseHandle(myFile);
}

// Загрузить файл на клиент
bool DepositDatabase::load() {
    Deposit::D record;
    int counts;

    if (!ReadFile(hPipe, (LPVOID)&counts, sizeof(counts), &bytesRead, NULL)) {
        return false;
    }

    for (int i = 0; i < counts; ++i)
    {
        // id
        ReadFile(hPipe, (LPVOID)&record.id, sizeof(record.id), &bytesRead, NULL);
        // Номер счета
        ReadFile(hPipe, (LPVOID)&record.accountNumber, sizeof(record.accountNumber), &bytesRead, NULL);
        // Тип вклада
        ReadFile(hPipe, (LPVOID)&record.type, sizeof(record.type), &bytesRead, NULL);
        // ФИО
        ReadFile(hPipe, (LPVOID)&record.FIO, sizeof(record.FIO), &bytesRead, NULL);
        // Дата рождения
        ReadFile(hPipe, (LPVOID)&record.birthDate, sizeof(record.birthDate), &bytesRead, NULL);
        // Сумма вклада
        ReadFile(hPipe, (LPVOID)&record.amount, sizeof(record.amount), &bytesRead, NULL);
        // Процент вклада
        ReadFile(hPipe, (LPVOID)&record.interest, sizeof(record.interest), &bytesRead, NULL);
        // Переодичность начисления
        ReadFile(hPipe, (LPVOID)&record.accrualFrequency, sizeof(record.accrualFrequency), &bytesRead, NULL);
        // Последняя транзакция
        ReadFile(hPipe, (LPVOID)&record.lastTransaction, sizeof(record.lastTransaction), &bytesRead, NULL);
        // Наличие пластиковой карты
        ReadFile(hPipe, (LPVOID)&record.plasticCardAvailability, sizeof(record.plasticCardAvailability), &bytesRead, NULL);

        Deposit deposit = Deposit::fromStruct(record);
        int pos = append(deposit);

        WriteFile(hPipe, (LPCVOID)&deposit.id, sizeof(unsigned int), &bytesWritten, NULL);
        WriteFile(hPipe, (LPCVOID)&pos, sizeof(int), &bytesWritten, NULL);
    }

    return 1;
}

// Добавить элемент
int DepositDatabase::add(const Deposit& record) {

    QVector<Deposit>::const_iterator i = database.constBegin();

    while (i != database.constEnd() && *i > record)
        ++i;

    if (i == database.constEnd()) {
        database.append(record);
        return database.count()-1;
    }

    int row = (i - database.constBegin());

    database.insert(i, record);

    return row;
}

// Добавить элемент в базу данных
int DepositDatabase::append(Deposit& record) {
    record.id = ++id;

    return add(record);
}

// Удаленить элемент из базы данных
void DepositDatabase::remove(unsigned int id) {
    QVector <Deposit>::const_iterator i;

    for (i = database.constBegin(); i != database.constEnd(); ++i) {
        if (i->id == id) {
                database.erase(i);
                return;
        }
    }
}

// Обновить запись в базе данных
int DepositDatabase::update(const Deposit& record) {
    remove(record.id);

    return add(record);
}

// Возвратить вектор записей
const QVector<DepositDatabase::RecordRow> DepositDatabase::records() const {
    QVector<RecordRow> rr;

    QVector<Deposit>::const_iterator i;

    for (i = database.constBegin(); i != database.constEnd(); ++i)
        rr.append({
            i->id,
            i->accountNumber,
            i->amount
        });

    return rr;
}

// Открыть запись для чтения
void DepositDatabase::record(unsigned int id, Deposit &record) const {
    QVector<Deposit>::const_iterator i = database.constBegin();

    while (i != database.constEnd() && i->id != id)
        ++i;

    if (i == database.constEnd())
        return;

    record = *i;
}

// Кол-во элементов
int DepositDatabase::count() {
    return database.count();
}

// Уничтожение всех данных
void DepositDatabase::clear() {
    database.clear();
}
