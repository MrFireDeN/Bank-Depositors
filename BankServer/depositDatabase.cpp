#include "depositDatabase.h"
#include <iostream>

using namespace std;

DepositDatabase::DepositDatabase()
{

}

// Запустить сервер
bool DepositDatabase::start() {
    // Считать данные из файла
    if (!load()) {
        qDebug() << "Ошибка при загрузке базы данных: " << GetLastError();
        CloseHandle(hPipe);
        return 0;
    }
    qDebug() << "База данных успешно загружена.\n";

    // Создание именованного канала для приема подключений
    HANDLE hServerPipe = CreateNamedPipe(
        SERVERPIPE,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_BYTE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        1024,
        1024,
        5000,
        NULL
        );

    if (hServerPipe == INVALID_HANDLE_VALUE) {
        qDebug() << "Ошибка создания канала: " << GetLastError();
        return 0;
    }
    qDebug() << "Именованный канал успешно создан.\n";

    // Цикл подключения клиентов
    while (true) {
        // Ожидание соединения
        if (!ConnectNamedPipe(hServerPipe, NULL)) {
            qDebug() << "Ошибка при ожидании подключения: " << GetLastError();
            CloseHandle(hServerPipe);
            return 0;
        }
        qDebug() << "Успешное подключение.\n";

        // Создание нового именованного канала для взаимодействия с клиентом
        std::wstring clientPipeName = SERVERPIPE;
        clientPipeName += std::to_wstring(GetCurrentThreadId());

        HANDLE hClientPipe = CreateNamedPipe(
            clientPipeName.c_str(),
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_BYTE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            1024,
            1024,
            5000,
            NULL
            );

        if (hClientPipe == INVALID_HANDLE_VALUE) {
            qDebug() << "Ошибка создания канала для клиента: " << GetLastError();
            CloseHandle(hServerPipe);
            return 0;
        }
        qDebug() << "Канал для клиента успешно создан.\n";

        // Создание потока для обслуживания клиента
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&DepositDatabase::handleClient, (LPVOID)hClientPipe, 0, NULL);

        QThread::sleep(1000);
        qDebug() << "Ожидание следующего клиента.\n";

        // Закрытие канала приема подключений
        DisconnectNamedPipe(hServerPipe);
    }

    return 1;
}

DWORD WINAPI DepositDatabase::handleClient(LPVOID lpParam) {
    DepositDatabase* db = static_cast<DepositDatabase*>(lpParam);
    db->handleClientLogic(lpParam);

    return 0;
}

// Функция для обслуживания клиента в отдельном потоке
void DepositDatabase::handleClientLogic(LPVOID lpParam) {
    HANDLE hClientPipe = (HANDLE)lpParam;

    const DWORD
        FINISH_REQ  = 0,
        APPEND_REQ  = 1,
        REMOVE_REQ  = 2,
        SAVE_REQ    = 3,
        RECORD_REQ  = 4,
        RECORDS_REQ = 5,
        COUNT_REQ   = 6,
        UPDATE_REQ  = 7;

    int req;
    DWORD bytesRead;

    do {
        // Загрузка номера запроса
        ReadFile(hClientPipe, (LPVOID)&req, sizeof(int), &bytesRead, NULL);

        switch (req) {
        case APPEND_REQ:
            append();
            break;
        case REMOVE_REQ:
            remove();
            break;
        case SAVE_REQ:
            save();
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
        case UPDATE_REQ:
            update();
            break;
        }
    } while (req != FINISH_REQ);

    qDebug() << "Отключение успешно.\n";
    CloseHandle(hClientPipe);
}

/*
while (true) {
    // Создание именованного канала для приема подключений
    HANDLE hServerPipe = CreateNamedPipe(
        SERVERPIPE,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_BYTE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        1024,
        1024,
        5000,
        NULL
    );

    if (hServerPipe == INVALID_HANDLE_VALUE) {
        qDebug() << "Error creating server channel: " << GetLastError();
        return 0;
    }
    qDebug() << "Server channel created successfully.\n";

    // Ожидание подключения клиента к каналу приема подключений
    if (!ConnectNamedPipe(hServerPipe, NULL)) {
        qDebug() << "Error while waiting for connection: " << GetLastError();
        CloseHandle(hServerPipe);
        return 0;
    }
    qDebug() << "Client connected to server channel.\n";

    // Генерация уникального имени для канала взаимодействия с клиентом
    std::string clientPipeName = "ClientPipe_" + std::to_string(GetCurrentThreadId());

    // Создание именованного канала для взаимодействия с клиентом
    HANDLE hClientPipe = CreateNamedPipe(
        clientPipeName.c_str(),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_BYTE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        1024,
        1024,
        5000,
        NULL
    );

    if (hClientPipe == INVALID_HANDLE_VALUE) {
        qDebug() << "Error creating client channel: " << GetLastError();
        CloseHandle(hServerPipe);
        return 0;
    }
    qDebug() << "Client channel created successfully.\n";

    // Создание потока для обслуживания клиента
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&HandleClientThread, (LPVOID)hClientPipe, 0, NULL);

    // Закрытие канала приема подключений
    DisconnectNamedPipe(hServerPipe);
    CloseHandle(hServerPipe);
}
*/


// Загрузить файл c клиента
bool DepositDatabase::append() {
    Deposit::D record;

    // id
    //ReadFile(hPipe, (LPVOID)&record.id, sizeof(record.id), &bytesRead, NULL);
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
    pos = add(deposit);

    WriteFile(hPipe, (LPCVOID)&deposit.id, sizeof(unsigned int), &bytesWritten, NULL);
    WriteFile(hPipe, (LPCVOID)&pos, sizeof(int), &bytesWritten, NULL);

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
    */


    unsigned int tempID;
    ReadFile(hPipe, (LPVOID)&tempID, sizeof(tempID), &bytesRead, NULL);

    QVector <Deposit>::const_iterator i;

    for (i = database.constBegin(); i != database.constEnd(); ++i) {
        if (i->id == tempID) {
                database.erase(i);
                return 1;
        }
    }

    return 1;
}

bool DepositDatabase::update() {
    if (!remove()){
        return 0;
    }

    Deposit::D record;

    // id
    //ReadFile(hPipe, (LPVOID)&record.id, sizeof(record.id), &bytesRead, NULL);
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
    pos = add(deposit);

    WriteFile(hPipe, (LPCVOID)&pos, sizeof(int), &bytesWritten, NULL);

    return 1;
}

// Загрузить файл базы данных
bool DepositDatabase::load() {
    // Создание файла
    HANDLE myFile = CreateFile(
        FILENAME,               // Имя файла
        GENERIC_READ,           // Желаемый доступ к файлу (здесь только чтение)
        FILE_SHARE_READ | FILE_SHARE_WRITE,                      // Режим разделения (нельзя открывать другим процессам)
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

    if (!bytesRead) {
        CloseHandle(myFile);
        return 1;
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

        add(Deposit::fromStruct(d));
        id = d.id;
    }

    CloseHandle(myFile);

    return true;
}

// Сохранить файл базы данных
bool DepositDatabase::save() {
    // Создание файла
    HANDLE myFile = CreateFile(
        FILENAME,               // Имя файла
        GENERIC_WRITE,          // Желаемый доступ к файлу (здесь только запись)
        FILE_SHARE_READ | FILE_SHARE_WRITE,                      // Режим разделения (нельзя открывать другим процессам)
        NULL,                   // Атрибуты безопасности (не используется)
        CREATE_ALWAYS,          // Режим открытия (открываем существующий файл)
        FILE_ATTRIBUTE_NORMAL,  // Атрибуты файла (обычный файл)
        NULL                    // Дескриптор файла-шаблона (не используется)
        );

    // Проверка на успешность открытия файла
    if (myFile == INVALID_HANDLE_VALUE)
        return false;

    // Проверка базы дпнных на заполненность
    if (database.empty()){
        CloseHandle(myFile);
        return false;
    }

    QVector<Deposit>::iterator i;
    Deposit::D d;
    DWORD bytesRead;
    int counts = database.count();

    WriteFile(myFile, &counts, sizeof(counts), &bytesRead, NULL);


    for (i = database.end()-1; i != database.begin()-1; --i) {
        d = Deposit::toStruct(*i);

        // id
        WriteFile(myFile, &d.id, sizeof(d.id), &bytesRead, NULL);
        // Номер счета
        WriteFile(myFile, &d.accountNumber, sizeof(d.accountNumber), &bytesRead, NULL);
        // Тип вклада
        WriteFile(myFile, &d.type, sizeof(d.type), &bytesRead, NULL);
        // ФИО
        WriteFile(myFile, &d.FIO, sizeof(d.FIO), &bytesRead, NULL);
        // Дата рождения
        WriteFile(myFile, &d.birthDate, sizeof(d.birthDate), &bytesRead, NULL);
        // Сумма вклада
        WriteFile(myFile, &d.amount, sizeof(d.amount), &bytesRead, NULL);
        // Процент вклада
        WriteFile(myFile, &d.interest, sizeof(d.interest), &bytesRead, NULL);
        // Переодичность начисления
        WriteFile(myFile, &d.accrualFrequency, sizeof(d.accrualFrequency), &bytesRead, NULL);
        // Последняя транзакция
        WriteFile(myFile, &d.lastTransaction, sizeof(d.lastTransaction), &bytesRead, NULL);
        // Наличие пластиковой карты
        WriteFile(myFile, &d.plasticCardAvailability, sizeof(d.plasticCardAvailability), &bytesRead, NULL);
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

    unsigned int tempID;
    ReadFile(hPipe, (LPVOID)&tempID, sizeof(tempID), &bytesRead, NULL);

    QVector<Deposit>::iterator i = database.begin();

    while (i != database.end() && i->id != tempID)
        ++i;

//    if (i == database.end())
//        return 0;

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

    for (i = database.end()-1; i != database.begin()-1; --i){
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
