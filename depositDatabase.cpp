#include "depositDatabase.h"

DepositDatabase::DepositDatabase()
{

}

// Добавить элемент
int DepositDatabase::add(const Deposit& record) {
//    QVector<Deposit>::const_iterator i = database.constBegin();

//    while (i != database.constEnd() && *i > record)
//        ++i;

//    if (i == database.constEnd()) {
//        database.append(record);
//        return database.count()-1;
//    }

//    int row = (i - database.constBegin());

//    database.insert(i, record);

//    return row;

    return 0;
}

// Добавить элемент в базу данных
int DepositDatabase::append(Deposit& record) {
    record.id = ++id;

    return add(record);
}

// Удаленить элемент из базы данных
void DepositDatabase::remove(unsigned int id) {
//    QVector <Deposit>::const_iterator i;

//    for (i = database.constBegin(); i != database.constEnd(); ++i) {
//        if (i->id == id) {
//            database.erase(i);
//            return;
//        }
//    }
}

// Обновить запись в базе данных
int DepositDatabase::update(const Deposit& record) {
    remove(record.id);

    return add(record);
}

// Возвратить вектор записей
const QVector<DepositDatabase::RecordRow> DepositDatabase::records() const {
    QVector<RecordRow> rr;

//    QVector<Deposit>::const_iterator i;

//    for (i = database.constBegin(); i != database.constEnd(); ++i)
//        rr.append({
//            i->id,
//            i->accountNumber,
//            i->amount
//        });

    return rr;
}

// Открыть запись для чтения
void DepositDatabase::record(unsigned int id, Deposit &record) const {
//    QVector<Deposit>::const_iterator i = database.constBegin();

//    while (i != database.constEnd() && i->id != id)
//        ++i;

//    if (i == database.constEnd())
//        return;

//    record = *i;
}

// Подключение к серверу
bool DepositDatabase::connect() {
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);

    // Создание нового процесса
    if (!CreateProcess(SERVERNAME, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        qDebug() << "Ошибка при запуске сервера: " << GetLastError();
        return 0;
    }

    qDebug() << "Сервер успешно запущен.\n";

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
        qDebug() << "Ошибка при подключении к каналу: " << GetLastError();
        return 0;
    }

    qDebug() << "Подключение к каналу прошло успешно.";

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

// Сохранить файл на сервер
bool DepositDatabase::save() {
    // Изменение режима канала на запись
    mode = PIPE_TYPE_MESSAGE | PIPE_WAIT;
    if (!SetNamedPipeHandleState(hPipe, &mode, NULL, NULL)) {
        qDebug() << "Ошибка при установке режима канала на запись: " << GetLastError();
        CloseHandle(hPipe);
        return 0;
    }
    qDebug() << "Режим канала на запись установлен.";

    // Пример чтения из канала
    char buffer[1024];
    DWORD bytesWrite;
    if (!WriteFile(hPipe, buffer, sizeof(buffer), &bytesWrite, NULL)) {
        qDebug() << "Ошибка при чтении из канала: " << GetLastError();
        CloseHandle(hPipe);
        return 0;
    }

    qDebug() << "Прочитано из канала: " << buffer;

    return 1;
}

// Загрузить файл с сервера
bool DepositDatabase::load() {
    // Изменение режима канала на чтение
    mode = PIPE_READMODE_BYTE | PIPE_WAIT;
    if (!SetNamedPipeHandleState(hPipe, &mode, NULL, NULL)) {
        qDebug() << "Ошибка при установке режима канала на чтение: " << GetLastError();
        CloseHandle(hPipe);
        return 0;
    }
    qDebug() << "Режим канала на чтение установлен.";

    // Пример чтения из канала
    char buffer[1024];
    DWORD bytesRead;
    if (!ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL)) {
        qDebug() << "Ошибка при записи в канал: " << GetLastError();
        CloseHandle(hPipe);
        return 0;
    }

    qDebug() << "Записано в канал: " << buffer;

    return 1;
}

// Кол-во элементов
int DepositDatabase::count() {
    //return database.count();

    return 0;
}

// Уничтожение всех данных
void DepositDatabase::clear() {
    //database.clear();
}
