#include "depositDatabase.h"
#include <iostream>

using namespace std;

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
        qDebug() << "Error creating channel: " << GetLastError();
        //cout << "Error creating channel: " << GetLastError();
        return 0;
    }

    qDebug() << "Named pipe created successfully.\n";
    //cout << "Named pipe created successfully.\n";

    // Ожидание соединения
    if (!ConnectNamedPipe(hPipe, NULL)) {
        qDebug() << "Error while waiting for connection: " << GetLastError();
        cout << "Error while waiting for connection: " << GetLastError();
        CloseHandle(hPipe);
        return 0;
    }

    if (!load()) {
        qDebug() << "Error while loading database: " << GetLastError();
        CloseHandle(hPipe);
        return 0;
    }
    qDebug() << "Database successfully load.\n";

    const DWORD
        FINISH_REQ  = 0,
        APPEND_REQ  = 1,
        REMOVE_REQ  = 2,
        SEND_REQ    = 3,
        LOAD_REQ    = 4,
        RECORD_REQ  = 5,
        RECORDS_REQ = 6,
        COUNT_REQ   = 7;

    Deposit rec;
    do {
        //Загрузка номера запроса
        ReadFile(hPipe, (LPVOID)&req, sizeof(int), &bytesRead, NULL);

        switch (req) {
            case APPEND_REQ:
                append();
                break;
            case REMOVE_REQ:
                remove();
                break;
            case SEND_REQ:
                send();
                break;
            case LOAD_REQ:
                load();
                break;
            case RECORD_REQ:
                record();
                break;
            case RECORDS_REQ:
                records();
                break;
            case COUNT_REQ:
                count();
                break;
        }
    } while (req != FINISH_REQ);

    return 1;
}


// Загрузить файл c клиента
bool DepositDatabase::append() {
    Deposit::D record;
    int counts;

    if (!ReadFile(hPipe, (LPVOID)&counts, sizeof(counts), &bytesRead, NULL)) {
        return false;
    }

    clear();

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
        deposit.id = ++id;
        int pos = add(deposit);

        WriteFile(hPipe, (LPCVOID)&deposit.id, sizeof(unsigned int), &bytesWritten, NULL);
        WriteFile(hPipe, (LPCVOID)&pos, sizeof(int), &bytesWritten, NULL);
    }

    return 1;
}

// Отправить данные на клиент
bool DepositDatabase::send() {
    QVector<Deposit>::iterator i;
    Deposit::D record;
    int counts = database.count();

    //Проверка базы дпнных на заполненность
    if (database.empty())
    {
        return false;
    }

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

// Удаленить элемент из базы данных
bool DepositDatabase::remove() {
    /*
    1. Удаляет запись из списка
    2. Возвращает новую завись
    */

//    QVector <Deposit>::const_iterator i;

//    for (i = database.constBegin(); i != database.constEnd(); ++i) {
//        if (i->id == id) {
//                database.erase(i);
//                return;
//        }
//    }

    return 1;
}

// Загрузить файл базы данных
bool DepositDatabase::load() {
    // Создание файла
    HANDLE myFile = CreateFile(
        FILENAME,               // Имя файла
        GENERIC_READ,           // Желаемый доступ к файлу (здесь только чтение)
        0,                      // Режим разделения (нельзя открывать другим процессам)
        NULL,                   // Атрибуты безопасности (не используется)
        OPEN_ALWAYS,            // Режим создания (создаем новый файл или перезаписываем существующий)
        FILE_ATTRIBUTE_NORMAL,  // Атрибуты файла (обычный файл)
        NULL                    // Дескриптор файла-шаблона (не используется)
        );

    // Проверка на успешность открытия файла
    if (myFile == INVALID_HANDLE_VALUE)
        return false;

    // Очищаем локальную базу перед загрузкой
    database.clear();

    //QVector<Deposit>::iterator i;
    Deposit::D d;
    DWORD bytesRead;
    int counts;

    if (!ReadFile(myFile, (void*)&counts, sizeof(counts), &bytesRead, NULL)) {
        CloseHandle(myFile);
        return false;
    }

    for (int i = 0; i < counts; ++i)
    {
        // id
        ReadFile(myFile, (void*)&d.id, sizeof(d.id), &bytesRead, NULL);
        // Номер счета
        ReadFile(myFile, &d.accountNumber, sizeof(d.accountNumber), &bytesRead, NULL);
        // Тип вклада
        ReadFile(myFile, &d.type, sizeof(d.type), &bytesRead, NULL);
        // ФИО
        ReadFile(myFile, &d.FIO, sizeof(d.FIO), &bytesRead, NULL);
        // Дата рождения
        ReadFile(myFile, &d.birthDate, sizeof(d.birthDate), &bytesRead, NULL);
        // Сумма вклада
        ReadFile(myFile, &d.amount, sizeof(d.amount), &bytesRead, NULL);
        // Процент вклада
        ReadFile(myFile, &d.interest, sizeof(d.interest), &bytesRead, NULL);
        // Переодичность начисления
        ReadFile(myFile, &d.accrualFrequency, sizeof(d.accrualFrequency), &bytesRead, NULL);
        // Последняя транзакция
        ReadFile(myFile, &d.lastTransaction, sizeof(d.lastTransaction), &bytesRead, NULL);
        // Наличие пластиковой карты
        ReadFile(myFile, &d.plasticCardAvailability, sizeof(d.plasticCardAvailability), &bytesRead, NULL);

        database.append(Deposit::fromStruct(d));
    }

    CloseHandle(myFile);

    return true;
}

// Открыть запись для чтения
bool DepositDatabase::record(){
    /*
    1. Сервер получет id
    2. Cервер возвращает запись
    */

    ReadFile(hPipe, (LPVOID)&id, sizeof(int), &bytesRead, NULL);

    QVector<Deposit>::iterator i = database.begin();

    while (i != database.end() && i->id != id)
        ++i;

    if (i == database.end())
        return 0;

    Deposit::D d = Deposit::toStruct(*i);

    // id
    WriteFile(hPipe, &d.id, sizeof(d.id), &bytesRead, NULL);
    // Номер счета
    WriteFile(hPipe, &d.accountNumber, sizeof(d.accountNumber), &bytesRead, NULL);
    // Тип вклада
    WriteFile(hPipe, &d.type, sizeof(d.type), &bytesRead, NULL);
    // ФИ
    WriteFile(hPipe, &d.FIO, sizeof(d.FIO), &bytesRead, NULL);
    // Дата рождения
    WriteFile(hPipe, &d.birthDate, sizeof(d.birthDate), &bytesRead, NULL);
    // Сумма вклада
    WriteFile(hPipe, &d.amount, sizeof(d.amount), &bytesRead, NULL);
    // Процент вклада
    WriteFile(hPipe, &d.interest, sizeof(d.interest), &bytesRead, NULL);
    // Переодичность начисления
    WriteFile(hPipe, &d.accrualFrequency, sizeof(d.accrualFrequency), &bytesRead, NULL);
    // Последняя транзакция
    WriteFile(hPipe, &d.lastTransaction, sizeof(d.lastTransaction), &bytesRead, NULL);
    // Наличие пластиковой карты
    WriteFile(hPipe, &d.plasticCardAvailability, sizeof(d.plasticCardAvailability), &bytesRead, NULL);

    return 1;
}

// Возвратить вектор записей
bool DepositDatabase::records(){
    QVector<Deposit>::iterator i;
    Deposit::D d;

    int count = database.count();
    if (!WriteFile(hPipe, (LPVOID)&count, sizeof(count), &bytesWritten, NULL)) {
        qDebug() << "Error writing to channel: " << GetLastError();
        return 0;
    }

    for (i = database.begin(); i != database.end(); ++i){
        d = Deposit::toStruct(*i);

        WriteFile(hPipe, (LPVOID)&d.id, sizeof(d.id), &bytesWritten, NULL);
        WriteFile(hPipe, (LPVOID)&d.accountNumber, sizeof(d.accountNumber), &bytesWritten, NULL);
        WriteFile(hPipe, (LPVOID)&d.amount, sizeof(d.amount), &bytesWritten, NULL);
    }
    qDebug() << "Writting to chanel successfuly" ;

    return 1;
}

// Кол-во элементов
bool DepositDatabase::count() {
    int count = database.count();

//    intToBuffer(count, buffer);
//    if (!WriteFile(hPipe, (LPVOID)&buffer, sizeof(buffer), &bytesWritten, NULL)) {
//        qDebug() << "Error writing to channel: " << GetLastError();
//        return 0;
//    }

    if (!WriteFile(hPipe, (LPVOID)&count, sizeof(int), &bytesWritten, NULL)) {
        qDebug() << "Error writing to channel: " << GetLastError();
        return 0;
    }

    qDebug() << "Writting to chanel successfuly" ;
    return 1;
}

// Уничтожение всех данных
void DepositDatabase::clear() {
    database.clear();
}

void DepositDatabase::intToBuffer(int num, char buffer[1024]) {
    for (int i =0; i < bufferSize; ++i) {
        buffer[i] = '\0';
    }

    snprintf(buffer, sizeof(bufferSize), "%d", num);
}
