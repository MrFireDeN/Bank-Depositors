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

    DepositCPY recordCPY = toStruct(record);

    // Номер счета
    WriteFile(hPipe, &recordCPY.accountNumber, sizeof(recordCPY.accountNumber), &bytesRead, NULL);
    // Тип вклада
    WriteFile(hPipe, &recordCPY.type, sizeof(recordCPY.type), &bytesRead, NULL);
    // ФИО
    WriteFile(hPipe, &recordCPY.FIO, sizeof(recordCPY.FIO), &bytesRead, NULL);
    // Дата рождения
    WriteFile(hPipe, &recordCPY.birthDate, sizeof(recordCPY.birthDate), &bytesRead, NULL);
    // Сумма вклада
    WriteFile(hPipe, &recordCPY.amount, sizeof(recordCPY.amount), &bytesRead, NULL);
    // Процент вклада
    WriteFile(hPipe, &recordCPY.interest, sizeof(recordCPY.interest), &bytesRead, NULL);
    // Переодичность начисления
    WriteFile(hPipe, &recordCPY.accrualFrequency, sizeof(recordCPY.accrualFrequency), &bytesRead, NULL);
    // Последняя транзакция
    WriteFile(hPipe, &recordCPY.lastTransaction, sizeof(recordCPY.lastTransaction), &bytesRead, NULL);
    // Наличие пластиковой карты
    WriteFile(hPipe, &recordCPY.plasticCardAvailability, sizeof(recordCPY.plasticCardAvailability), &bytesRead, NULL);

    ReadFile(hPipe, (void*)&record.id, sizeof(record.id), &bytesRead, NULL);
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

    req = UPDATE_REQ;
    DWORD bytesWritten;
    if (!WriteFile(hPipe, (LPCVOID)&req, sizeof(int), &bytesWritten, NULL)) {
        qDebug() << "Error writing to channel: " << GetLastError();
        CloseHandle(hPipe);
        return -1;
    }

    WriteFile(hPipe, (LPCVOID)&record.id, sizeof(int), &bytesWritten, NULL);

    DepositCPY recordCPY = toStruct(recordCopy);

    // Номер счета
    WriteFile(hPipe, &recordCPY.accountNumber, sizeof(recordCPY.accountNumber), &bytesRead, NULL);
    // Тип вклада
    WriteFile(hPipe, &recordCPY.type, sizeof(recordCPY.type), &bytesRead, NULL);
    // ФИО
    WriteFile(hPipe, &recordCPY.FIO, sizeof(recordCPY.FIO), &bytesRead, NULL);
    // Дата рождения
    WriteFile(hPipe, &recordCPY.birthDate, sizeof(recordCPY.birthDate), &bytesRead, NULL);
    // Сумма вклада
    WriteFile(hPipe, &recordCPY.amount, sizeof(recordCPY.amount), &bytesRead, NULL);
    // Процент вклада
    WriteFile(hPipe, &recordCPY.interest, sizeof(recordCPY.interest), &bytesRead, NULL);
    // Переодичность начисления
    WriteFile(hPipe, &recordCPY.accrualFrequency, sizeof(recordCPY.accrualFrequency), &bytesRead, NULL);
    // Последняя транзакция
    WriteFile(hPipe, &recordCPY.lastTransaction, sizeof(recordCPY.lastTransaction), &bytesRead, NULL);
    // Наличие пластиковой карты
    WriteFile(hPipe, &recordCPY.plasticCardAvailability, sizeof(recordCPY.plasticCardAvailability), &bytesRead, NULL);


    ReadFile(hPipe, (void*)&pos, sizeof(pos), &bytesRead, NULL);

    return pos;
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

    DepositCPY recordCPY;
    Deposit deposit;

    for (int i = 0; i < count; ++i) {
        ReadFile(hPipe, (LPVOID)&recordCPY.id, sizeof(recordCPY.id), &bytesRead, NULL);
        ReadFile(hPipe, (LPVOID)&recordCPY.accountNumber, sizeof(recordCPY.accountNumber), &bytesRead, NULL);
        ReadFile(hPipe, (LPVOID)&recordCPY.amount, sizeof(recordCPY.amount), &bytesRead, NULL);

        rr.append(toRecord(recordCPY));
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
    DepositCPY recordCPY;

    // id
    ReadFile(hPipe, (void*)&recordCPY.id, sizeof(recordCPY.id), &bytesRead, NULL);
    // Номер счета
    ReadFile(hPipe, &recordCPY.accountNumber, sizeof(recordCPY.accountNumber), &bytesRead, NULL);
    // Тип вклада
    ReadFile(hPipe, &recordCPY.type, sizeof(recordCPY.type), &bytesRead, NULL);
    // ФИО
    ReadFile(hPipe, &recordCPY.FIO, sizeof(recordCPY.FIO), &bytesRead, NULL);
    // Дата рождения
    ReadFile(hPipe, &recordCPY.birthDate, sizeof(recordCPY.birthDate), &bytesRead, NULL);
    // Сумма вклада
    ReadFile(hPipe, &recordCPY.amount, sizeof(recordCPY.amount), &bytesRead, NULL);
    // Процент вклада
    ReadFile(hPipe, &recordCPY.interest, sizeof(recordCPY.interest), &bytesRead, NULL);
    // Переодичность начисления
    ReadFile(hPipe, &recordCPY.accrualFrequency, sizeof(recordCPY.accrualFrequency), &bytesRead, NULL);
    // Последняя транзакция
    ReadFile(hPipe, &recordCPY.lastTransaction, sizeof(recordCPY.lastTransaction), &bytesRead, NULL);
    // Наличие пластиковой карты
    ReadFile(hPipe, &recordCPY.plasticCardAvailability, sizeof(recordCPY.plasticCardAvailability), &bytesRead, NULL);

    record = fromStruct(recordCPY);
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
    qDebug() << "Written in channel: " << bytesWritten;

    // Пример чтения из канала
    int count;
    DWORD bytesRead;
    if (!ReadFile(hPipe, (LPVOID)&count, sizeof(int), &bytesRead, NULL)) {
        qDebug() << "Error read from channel: " << GetLastError();
        CloseHandle(hPipe);
        return 0;
    }

    qDebug() << "Read from the channel: " << count;

    return count;
}
