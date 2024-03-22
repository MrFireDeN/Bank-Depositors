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

// Открыть запись для чтения
bool DepositDatabase::record() const {
    /*
    1. Сервер получет id
    2. Cервер возвращает запись
    */

    //    QVector<Deposit>::const_iterator i = database.constBegin();

    //    while (i != database.constEnd() && i->id != id)
    //        ++i;

    //    if (i == database.constEnd())
    //        return;

    //    record = *i;

    return 1;
}

// Возвратить вектор записей
bool DepositDatabase::records() const {
    /*
    1. сервер возвращает вектор записей
    */

//    QVector<RecordRow> rr;

//    QVector<Deposit>::const_iterator i;

//    for (i = database.constBegin(); i != database.constEnd(); ++i)
//        rr.append({
//            i->id,
//            i->accountNumber,
//            i->amount
//        });

//    return rr;

    return 1;
}

// Кол-во элементов
bool DepositDatabase::count() {
    /*
    2. Cервер возвращает кол-во записей
    */


//    return database.count();

    return 1;
}

// Уничтожение всех данных
void DepositDatabase::clear() {
    database.clear();
}
