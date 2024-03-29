#include "depositDatabase.h"

DepositDatabase::DepositDatabase()
{

}

// Подключение к серверу
bool DepositDatabase::connect() {
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);

    // Создание нового процесса
    if (!CreateProcess(SERVERNAME, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        qDebug() << "Start server error: " << GetLastError();
        return 0;
    }

    qDebug() << "Start server successful.\n";

    QThread::msleep(2000);

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
    /*
    1. Отправляет запрос на добалвение элеманта на сервер
    2. сервер возвращает id добавленного элемента
    */

    int id = 0;
    return id;
}

// Удаленить элемент из базы данных
void DepositDatabase::remove(unsigned int id) {
    /*
    1. Отправляет запрос на удаление элеманта по id на сервер
    */
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
    /*
    1. Отправляет запрос на сервер на получение списка записей
    2. сервер возвращает вектор записей
    */

    QVector<RecordRow> rr;

    // Изменение режима канала на запись
//    mode = PIPE_TYPE_MESSAGE | PIPE_WAIT;
//    if (!SetNamedPipeHandleState(hPipe, &mode, NULL, NULL)) {
//        qDebug() << "Error when setting channel mode to writting: " << GetLastError();
//        CloseHandle(hPipe);
//        return rr;
//    }
//    qDebug() << "The channel mode is set to writting.";

    req = RECORDS_REQ;
    DWORD bytesWritten;
    if (!WriteFile(hPipe, (LPCVOID)&req, sizeof(int), &bytesWritten, NULL)) {
        qDebug() << "Error writing to channel: " << GetLastError();
        CloseHandle(hPipe);
        return rr;
    }

    // Изменение режима канала на чтение
//    mode = PIPE_READMODE_BYTE | PIPE_WAIT;
//    if (!SetNamedPipeHandleState(hPipe, &mode, NULL, NULL)) {
//        qDebug() << "Error when setting channel mode to read: " << GetLastError();
//        CloseHandle(hPipe);
//        return rr;
//    }
//    qDebug() << "Channel mode is set to read.";

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

        deposit = Deposit::fromStruct(d);

        rr.append({
            deposit.id,
            deposit.accountNumber,
            deposit.amount
        });
    }

    return rr;
}

// Открыть запись для чтения
void DepositDatabase::record(unsigned int id, Deposit &record) {
    /*
    1. Отправляет запрос на сервер на получение записи по id
    2. сервер возвращает запись
    */

    // Изменение режима канала на запись
//    mode = PIPE_TYPE_MESSAGE | PIPE_WAIT;
//    if (!SetNamedPipeHandleState(hPipe, &mode, NULL, NULL)) {
//        qDebug() << "Error when setting channel mode to writting: " << GetLastError();
//        CloseHandle(hPipe);
//        return;
//    }
//    qDebug() << "The channel mode is set to writting.";

    req = RECORD_REQ;
    DWORD bytesWritten;
    if (!WriteFile(hPipe, (LPCVOID)&req, sizeof(int), &bytesWritten, NULL)) {
        qDebug() << "Error writing to channel: " << GetLastError();
        CloseHandle(hPipe);
        return;
    }

    WriteFile(hPipe, (LPCVOID)&id, sizeof(int), &bytesWritten, NULL);

    // Изменение режима канала на чтение
//    mode = PIPE_READMODE_BYTE | PIPE_WAIT;
//    if (!SetNamedPipeHandleState(hPipe, &mode, NULL, NULL)) {
//        qDebug() << "Error when setting channel mode to read: " << GetLastError();
//        CloseHandle(hPipe);
//        return;
//    }
//    qDebug() << "Channel mode is set to read.";

    // Пример чтения из канала
    Deposit::D d;
    DWORD bytesRead;

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
    // Изменение режима канала на запись
//    mode = PIPE_TYPE_MESSAGE | PIPE_WAIT;
//    if (!SetNamedPipeHandleState(hPipe, &mode, NULL, NULL)) {
//        qDebug() << "Error when setting channel mode to writting: " << GetLastError();
//        CloseHandle(hPipe);
//        return 0;
//    }
//    qDebug() << "The channel mode is set to writting.";

    // Пример чтения из канала
    char buffer[1024];
    DWORD bytesWrite;
    if (!WriteFile(hPipe, buffer, sizeof(buffer), &bytesWrite, NULL)) {
        qDebug() << "Error reading from channel: " << GetLastError();
        CloseHandle(hPipe);
        return 0;
    }

    qDebug() << "Read from the channel: " << buffer;

    return 1;
}

// Загрузить файл с сервера
bool DepositDatabase::load() {
    return 1;
}

// Кол-во элементов
int DepositDatabase::count() {
    // Изменение режима канала на запись
//    mode = PIPE_TYPE_MESSAGE | PIPE_WAIT;
//    if (!SetNamedPipeHandleState(hPipe, &mode, NULL, NULL)) {
//        qDebug() << "Error when setting channel mode to writting: " << GetLastError();
//        CloseHandle(hPipe);
//        return 0;
//    }
//    qDebug() << "The channel mode is set to writting.";

    req = COUNT_REQ;
    DWORD bytesWritten;
    if (!WriteFile(hPipe, (LPCVOID)&req, sizeof(int), &bytesWritten, NULL)) {
        qDebug() << "Error writing to channel: " << GetLastError();
        CloseHandle(hPipe);
        return 0;
    }
    qDebug() << "Written in channel: " << buffer;

    // Изменение режима канала на чтение
//    mode = PIPE_READMODE_BYTE | PIPE_WAIT;
//    if (!SetNamedPipeHandleState(hPipe, &mode, NULL, NULL)) {
//        qDebug() << "Error when setting channel mode to read: " << GetLastError();
//        CloseHandle(hPipe);
//        return 0;
//    }
//    qDebug() << "Channel mode is set to read.";

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
