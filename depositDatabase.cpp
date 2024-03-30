#include "depositDatabase.h"

DepositDatabase::DepositDatabase()
{

}

// Подключение к серверу
bool DepositDatabase::connect() {
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);

//    // Создание нового процесса
//    if (!CreateProcess(SERVERNAME, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
//    {
//        qDebug() << "Start server error: " << GetLastError();
//        return 0;
//    }

    qDebug() << "Start server successful.\n";

    QThread::msleep(1000);

    hPipe = CreateFile(
        SERVERPIPE,                     // Имя канала
        GENERIC_READ | GENERIC_WRITE,   // Доступ к чтению и записи
        0,                              // Не разделять ресурс между потоками
        NULL,                           // Дескриптор безопасности по умолчанию
        OPEN_EXISTING,                  // Открыть существующий канал
        0,                              // Флаги и атрибуты по умолчанию
        NULL                            // Необходимость дополнительных атрибутов
        );

    if (hPipe == INVALID_HANDLE_VALUE) {
        qDebug() << "Connect error: " << GetLastError();
        return 0;
    }

    qDebug() << "Connect successful.";

    return 1;
}

// Отключение от серевера
bool DepositDatabase::disconnect() {
    req = FINISH_REQ;
    DWORD bytesWritten;
    if (!WriteFile(hPipe, (LPCVOID)&req, sizeof(int), &bytesWritten, NULL)) {
        qDebug() << "Error writing to channel: " << GetLastError();
        CloseHandle(hPipe);
        return 0;
    }

    // Закрытие дескриптора канала
    CloseHandle(hPipe);

    // Закрытие дескрипторов процесса и потока
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // Обнуление дескрипторов процесса и потока
    ZeroMemory(&pi, sizeof(pi));

    return 1;
}

// Добавить элемент в базу данных
int DepositDatabase::append(Deposit& record) {
    req = APPEND_REQ;
    if (!WriteFile(hPipe, (LPCVOID)&req, sizeof(int), &bytesWritten, NULL)) {
        qDebug() << "Error writing to channel: " << GetLastError();
        CloseHandle(hPipe);
        return -1;
    }

    Deposit::D d = Deposit::toStruct(record);

    // Номер счета
    WriteFile(hPipe, &d.accountNumber, sizeof(d.accountNumber), &bytesRead, NULL);
    // Тип вклада
    WriteFile(hPipe, &d.type, sizeof(d.type), &bytesRead, NULL);
    // ФИО
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

    record = Deposit::fromStruct(d);

    ReadFile(hPipe, (void*)&id, sizeof(id), &bytesRead, NULL);
    record.id = id;
    ReadFile(hPipe, (void*)&pos, sizeof(pos), &bytesRead, NULL);

    return pos;
}

// Удаленить элемент из базы данных
void DepositDatabase::remove(unsigned int id) {
    /*
    1. Отправляет запрос на удаление элеманта по id на сервер
    */

    req = REMOVE_REQ;
    if (!WriteFile(hPipe, (LPCVOID)&req, sizeof(int), &bytesWritten, NULL)) {
        qDebug() << "Error writing to channel: " << GetLastError();
        CloseHandle(hPipe);
        return;
    }

    WriteFile(hPipe, (LPCVOID)&id, sizeof(int), &bytesWritten, NULL);
}

// Обновить запись в базе данных
int DepositDatabase::update(const Deposit& record) {
    // Создаем копию объекта record
    Deposit recordCopy = record;

    // Удаляем старую запись из базы данных
    remove(recordCopy.id);

    // Добавляем обновленную запись в базу данных
    return append(recordCopy);
}

// Возвратить вектор записей
const QVector<DepositDatabase::RecordRow> DepositDatabase::records() {
    QVector<RecordRow> rr;

    req = RECORDS_REQ;
    DWORD bytesWritten;
    if (!WriteFile(hPipe, (LPCVOID)&req, sizeof(int), &bytesWritten, NULL)) {
        qDebug() << "Error writing to channel: " << GetLastError();
        CloseHandle(hPipe);
        return rr;
    }

    // Пример чтения из канала
    int count;
    DWORD bytesRead;
    if (!ReadFile(hPipe, (LPVOID)&count, sizeof(int), &bytesRead, NULL)) {
        qDebug() << "Error read from channel: " << GetLastError();
        CloseHandle(hPipe);
        return rr;
    }
    qDebug() << "Read from the channel: " << count;

    Deposit::D d;
    Deposit deposit;

    for (int i = 0; i < count; ++i) {
        ReadFile(hPipe, (LPVOID)&d.id, sizeof(d.id), &bytesRead, NULL);
        ReadFile(hPipe, (LPVOID)&d.accountNumber, sizeof(d.accountNumber), &bytesRead, NULL);
        ReadFile(hPipe, (LPVOID)&d.amount, sizeof(d.amount), &bytesRead, NULL);

        rr.append(DepositDatabase::toRecord(d));
    }

    return rr;
}

// Открыть запись для чтения
void DepositDatabase::record(unsigned int id, Deposit &record) {
    req = RECORD_REQ;
    DWORD bytesWritten;
    if (!WriteFile(hPipe, (LPCVOID)&req, sizeof(int), &bytesWritten, NULL)) {
        qDebug() << "Error writing to channel: " << GetLastError();
        CloseHandle(hPipe);
        return;
    }

    WriteFile(hPipe, (LPCVOID)&id, sizeof(id), &bytesWritten, NULL);

    // Пример чтения из канала
    Deposit::D d;

    // id
    ReadFile(hPipe, (void*)&d.id, sizeof(d.id), &bytesRead, NULL);
    // Номер счета
    ReadFile(hPipe, &d.accountNumber, sizeof(d.accountNumber), &bytesRead, NULL);
    // Тип вклада
    ReadFile(hPipe, &d.type, sizeof(d.type), &bytesRead, NULL);
    // ФИО
    ReadFile(hPipe, &d.FIO, sizeof(d.FIO), &bytesRead, NULL);
    // Дата рождения
    ReadFile(hPipe, &d.birthDate, sizeof(d.birthDate), &bytesRead, NULL);
    // Сумма вклада
    ReadFile(hPipe, &d.amount, sizeof(d.amount), &bytesRead, NULL);
    // Процент вклада
    ReadFile(hPipe, &d.interest, sizeof(d.interest), &bytesRead, NULL);
    // Переодичность начисления
    ReadFile(hPipe, &d.accrualFrequency, sizeof(d.accrualFrequency), &bytesRead, NULL);
    // Последняя транзакция
    ReadFile(hPipe, &d.lastTransaction, sizeof(d.lastTransaction), &bytesRead, NULL);
    // Наличие пластиковой карты
    ReadFile(hPipe, &d.plasticCardAvailability, sizeof(d.plasticCardAvailability), &bytesRead, NULL);

    record = Deposit::fromStruct(d);
}

// Сохранить файл на сервер
bool DepositDatabase::save() {
    req = SAVE_REQ;
    DWORD bytesWritten;
    if (!WriteFile(hPipe, (LPCVOID)&req, sizeof(int), &bytesWritten, NULL)) {
        qDebug() << "Error writing to channel: " << GetLastError();
        CloseHandle(hPipe);
        return 0;
    }

    return 1;
}

// Загрузить файл с сервера
bool DepositDatabase::load() {
    return 1;
}

// Кол-во элементов
int DepositDatabase::count() {
    req = COUNT_REQ;
    DWORD bytesWritten;
    if (!WriteFile(hPipe, (LPCVOID)&req, sizeof(int), &bytesWritten, NULL)) {
        qDebug() << "Error writing to channel: " << GetLastError();
        CloseHandle(hPipe);
        return 0;
    }
    qDebug() << "Written in channel: " << buffer;

    // Пример чтения из канала
    int number;
    DWORD bytesRead;
    if (!ReadFile(hPipe, (LPVOID)&number, sizeof(int), &bytesRead, NULL)) {
        qDebug() << "Error read from channel: " << GetLastError();
        CloseHandle(hPipe);
        return 0;
    }

    qDebug() << "Read from the channel: " << number;

    return number;
}
R
DepositDatabase::RecordRow DepositDatabase::toRecord(Deposit::D d) {
    DepositDatabase::RecordRow rr;

    rr.id = d.id;
    rr.amount = d.amount;
    rr.accountNumber = QString::fromStdString(d.accountNumber);

    return rr;
}

Deposit::D DepositDatabase::fromRecord(DepositDatabase::RecordRow rr) {
    Deposit::D d;

    d.id = rr.id;
    d.amount = rr.amount;

    QByteArray stringData = rr.accountNumber.toUtf8();
    std::copy(stringData.constBegin(), stringData.constBegin()+qMin(20, stringData.size()), d.accountNumber);

    return d;
}
