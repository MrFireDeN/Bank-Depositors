#include "depositDatabase.h"

DepositDatabase::DepositDatabase()
{
    // Считать данные из файла
    if (!load()) {
        qDebug() << "Ошибка при загрузке базы данных: " << GetLastError();
    }
    qDebug() << "Базы данных успешно загружена!";
}

DepositDatabase::~DepositDatabase() {
    save();
}

// Загрузить файл c клиента
bool DepositDatabase::append(SOCKET sock) {
    DepositCPY recordCPY;

    // // Номер счета
    // ReadFile(hPipe, (LPVOID)&recordCPY.accountNumber, sizeof(recordCPY.accountNumber), &bytesRead, NULL);
    // // Тип вклада
    // ReadFile(hPipe, (LPVOID)&recordCPY.type, sizeof(recordCPY.type), &bytesRead, NULL);
    // // ФИО
    // ReadFile(hPipe, (LPVOID)&recordCPY.FIO, sizeof(recordCPY.FIO), &bytesRead, NULL);
    // // Дата рождения
    // ReadFile(hPipe, (LPVOID)&recordCPY.birthDate, sizeof(recordCPY.birthDate), &bytesRead, NULL);
    // // Сумма вклада
    // ReadFile(hPipe, (LPVOID)&recordCPY.amount, sizeof(recordCPY.amount), &bytesRead, NULL);
    // // Процент вклада
    // ReadFile(hPipe, (LPVOID)&recordCPY.interest, sizeof(recordCPY.interest), &bytesRead, NULL);
    // // Переодичность начисления
    // ReadFile(hPipe, (LPVOID)&recordCPY.accrualFrequency, sizeof(recordCPY.accrualFrequency), &bytesRead, NULL);
    // // Последняя транзакция
    // ReadFile(hPipe, (LPVOID)&recordCPY.lastTransaction, sizeof(recordCPY.lastTransaction), &bytesRead, NULL);
    // // Наличие пластиковой карты
    // ReadFile(hPipe, (LPVOID)&recordCPY.plasticCardAvailability, sizeof(recordCPY.plasticCardAvailability), &bytesRead, NULL);

    // Deposit record = fromStruct(recordCPY);
    // record.id = ++id;
    // pos = add(record);

    // WriteFile(hPipe, (LPCVOID)&record.id, sizeof(unsigned int), &bytesWritten, NULL);
    // WriteFile(hPipe, (LPCVOID)&pos, sizeof(int), &bytesWritten, NULL);

    recv(sock, (char*)&recordCPY, sizeof(recordCPY), 0);

    Deposit record = fromStruct(recordCPY);
    record.id = ++id;
    pos = add(record);

    send(sock, (char*)&record.id, sizeof(unsigned int), 0);
    send(sock, (char*)&pos, sizeof(int), 0);

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
bool DepositDatabase::remove(SOCKET sock) {
    unsigned int tempID;
    //ReadFile(hPipe, (LPVOID)&tempID, sizeof(tempID), &bytesRead, NULL);
    recv(sock, (char*)&tempID, sizeof(tempID), 0);

    QVector <Deposit>::const_iterator i;

    for (i = database.constBegin(); i != database.constEnd(); ++i) {
        if (i->id == tempID) {
                database.erase(i);
                return 1;
        }
    }

    return 1;
}

bool DepositDatabase::update(SOCKET sock) {
    DepositCPY recordCPY;

    // // id
    // ReadFile(hPipe, (LPVOID)&recordCPY.id, sizeof(recordCPY.id), &bytesRead, NULL);
    // // Номер счета
    // ReadFile(hPipe, (LPVOID)&recordCPY.accountNumber, sizeof(recordCPY.accountNumber), &bytesRead, NULL);
    // // Тип вклада
    // ReadFile(hPipe, (LPVOID)&recordCPY.type, sizeof(recordCPY.type), &bytesRead, NULL);
    // // ФИО
    // ReadFile(hPipe, (LPVOID)&recordCPY.FIO, sizeof(recordCPY.FIO), &bytesRead, NULL);
    // // Дата рождения
    // ReadFile(hPipe, (LPVOID)&recordCPY.birthDate, sizeof(recordCPY.birthDate), &bytesRead, NULL);
    // // Сумма вклада
    // ReadFile(hPipe, (LPVOID)&recordCPY.amount, sizeof(recordCPY.amount), &bytesRead, NULL);
    // // Процент вклада
    // ReadFile(hPipe, (LPVOID)&recordCPY.interest, sizeof(recordCPY.interest), &bytesRead, NULL);
    // // Переодичность начисления
    // ReadFile(hPipe, (LPVOID)&recordCPY.accrualFrequency, sizeof(recordCPY.accrualFrequency), &bytesRead, NULL);
    // // Последняя транзакция
    // ReadFile(hPipe, (LPVOID)&recordCPY.lastTransaction, sizeof(recordCPY.lastTransaction), &bytesRead, NULL);
    // // Наличие пластиковой карты
    // ReadFile(hPipe, (LPVOID)&recordCPY.plasticCardAvailability, sizeof(recordCPY.plasticCardAvailability), &bytesRead, NULL);

    recv(sock, (char*)&recordCPY, sizeof(recordCPY), 0);

    Deposit record = fromStruct(recordCPY);

    QVector <Deposit>::const_iterator i;

    for (i = database.constBegin(); i != database.constEnd(); ++i) {
        if (i->id == record.id) {
                database.erase(i);
                pos = add(record);
                send(sock, (char*)&pos, sizeof(int), 0);
                return 1;
        }
    }

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
    DepositCPY recordCPY;
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
        ReadFile(myFile, (void*)&recordCPY.id, sizeof(recordCPY.id), &bytesRead, NULL);
        // Номер счета
        ReadFile(myFile, &recordCPY.accountNumber, sizeof(recordCPY.accountNumber), &bytesRead, NULL);
        // Тип вклада
        ReadFile(myFile, &recordCPY.type, sizeof(recordCPY.type), &bytesRead, NULL);
        // ФИО
        ReadFile(myFile, &recordCPY.FIO, sizeof(recordCPY.FIO), &bytesRead, NULL);
        // Дата рождения
        ReadFile(myFile, &recordCPY.birthDate, sizeof(recordCPY.birthDate), &bytesRead, NULL);
        // Сумма вклада
        ReadFile(myFile, &recordCPY.amount, sizeof(recordCPY.amount), &bytesRead, NULL);
        // Процент вклада
        ReadFile(myFile, &recordCPY.interest, sizeof(recordCPY.interest), &bytesRead, NULL);
        // Переодичность начисления
        ReadFile(myFile, &recordCPY.accrualFrequency, sizeof(recordCPY.accrualFrequency), &bytesRead, NULL);
        // Последняя транзакция
        ReadFile(myFile, &recordCPY.lastTransaction, sizeof(recordCPY.lastTransaction), &bytesRead, NULL);
        // Наличие пластиковой карты
        ReadFile(myFile, &recordCPY.plasticCardAvailability, sizeof(recordCPY.plasticCardAvailability), &bytesRead, NULL);

        add(fromStruct(recordCPY));

        if (recordCPY.id && recordCPY.id > id) {
            id = recordCPY.id;
        }
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
    DepositCPY recordCPY;
    DWORD bytesRead;
    int counts = database.count();

    WriteFile(myFile, &counts, sizeof(counts), &bytesRead, NULL);


    for (i = database.end()-1; i != database.begin()-1; --i) {
        recordCPY = toStruct(*i);

        // id
        WriteFile(myFile, &recordCPY.id, sizeof(recordCPY.id), &bytesRead, NULL);
        // Номер счета
        WriteFile(myFile, &recordCPY.accountNumber, sizeof(recordCPY.accountNumber), &bytesRead, NULL);
        // Тип вклада
        WriteFile(myFile, &recordCPY.type, sizeof(recordCPY.type), &bytesRead, NULL);
        // ФИО
        WriteFile(myFile, &recordCPY.FIO, sizeof(recordCPY.FIO), &bytesRead, NULL);
        // Дата рождения
        WriteFile(myFile, &recordCPY.birthDate, sizeof(recordCPY.birthDate), &bytesRead, NULL);
        // Сумма вклада
        WriteFile(myFile, &recordCPY.amount, sizeof(recordCPY.amount), &bytesRead, NULL);
        // Процент вклада
        WriteFile(myFile, &recordCPY.interest, sizeof(recordCPY.interest), &bytesRead, NULL);
        // Переодичность начисления
        WriteFile(myFile, &recordCPY.accrualFrequency, sizeof(recordCPY.accrualFrequency), &bytesRead, NULL);
        // Последняя транзакция
        WriteFile(myFile, &recordCPY.lastTransaction, sizeof(recordCPY.lastTransaction), &bytesRead, NULL);
        // Наличие пластиковой карты
        WriteFile(myFile, &recordCPY.plasticCardAvailability, sizeof(recordCPY.plasticCardAvailability), &bytesRead, NULL);
    }

    CloseHandle(myFile);

    return true;
}

// Открыть запись для чтения
bool DepositDatabase::record(SOCKET sock){
    unsigned int tempID;
    //ReadFile(hPipe, (LPVOID)&tempID, sizeof(tempID), &bytesRead, NULL);
    recv(sock, (char*)&tempID, sizeof(tempID), 0);

    QVector<Deposit>::iterator i = database.begin();

    while (i != database.end()-1 && i->id != tempID)
        i++;

    DepositCPY recordCPY = toStruct(*i);

    // // id
    // WriteFile(hPipe, &recordCPY.id, sizeof(recordCPY.id), &bytesRead, NULL);
    // // Номер счета
    // WriteFile(hPipe, &recordCPY.accountNumber, sizeof(recordCPY.accountNumber), &bytesRead, NULL);
    // // Тип вклада
    // WriteFile(hPipe, &recordCPY.type, sizeof(recordCPY.type), &bytesRead, NULL);
    // // ФИО
    // WriteFile(hPipe, &recordCPY.FIO, sizeof(recordCPY.FIO), &bytesRead, NULL);
    // // Дата рождения
    // WriteFile(hPipe, &recordCPY.birthDate, sizeof(recordCPY.birthDate), &bytesRead, NULL);
    // // Сумма вклада
    // WriteFile(hPipe, &recordCPY.amount, sizeof(recordCPY.amount), &bytesRead, NULL);
    // // Процент вклада
    // WriteFile(hPipe, &recordCPY.interest, sizeof(recordCPY.interest), &bytesRead, NULL);
    // // Переодичность начисления
    // WriteFile(hPipe, &recordCPY.accrualFrequency, sizeof(recordCPY.accrualFrequency), &bytesRead, NULL);
    // // Последняя транзакция
    // WriteFile(hPipe, &recordCPY.lastTransaction, sizeof(recordCPY.lastTransaction), &bytesRead, NULL);
    // // Наличие пластиковой карты
    // WriteFile(hPipe, &recordCPY.plasticCardAvailability, sizeof(recordCPY.plasticCardAvailability), &bytesRead, NULL);

    // Отправка данных обратно клиенту
    send(sock, (char*)&recordCPY, sizeof(recordCPY), 0);

    return 1;
}

// Возвратить вектор записей
bool DepositDatabase::records(SOCKET sock){
    QVector<Deposit>::iterator i;
    DepositCPY recordCPY;

    int count = database.count();
    // if (!WriteFile(hPipe, (LPVOID)&count, sizeof(count), &bytesWritten, NULL)) {
    //     qDebug() << "Error writing to channel: " << GetLastError();
    //     return 0;
    // }
    send(sock, (char*)&count, sizeof(count), 0);

    for (i = database.begin(); i != database.end(); ++i) {
        recordCPY = toStruct(*i);

        // WriteFile(hPipe, (LPVOID)&recordCPY.id, sizeof(recordCPY.id), &bytesWritten, NULL);
        // WriteFile(hPipe, (LPVOID)&recordCPY.accountNumber, sizeof(recordCPY.accountNumber), &bytesWritten, NULL);
        // WriteFile(hPipe, (LPVOID)&recordCPY.amount, sizeof(recordCPY.amount), &bytesWritten, NULL);
        send(sock, (char*)&recordCPY.id, sizeof(recordCPY.id), 0);
        send(sock, (char*)&recordCPY.accountNumber, sizeof(recordCPY.accountNumber), 0);
        send(sock, (char*)&recordCPY.amount, sizeof(recordCPY.amount), 0);
    }

    return 1;
}

// Кол-во элементов
bool DepositDatabase::count(SOCKET sock) {
    int count = database.count();

    send(sock, (char*)&count, sizeof(count), 0);
    return 1;
}

// Уничтожение всех данных
void DepositDatabase::clear() {
    database.clear();
}
