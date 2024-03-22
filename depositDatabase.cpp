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
        qDebug() << "Ошибка при запуске сервера: " << GetLastError();
        return 0;
    }

    qDebug() << "Сервер успешно запущен.\n";

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
const QVector<DepositDatabase::RecordRow> DepositDatabase::records() const {
    QVector<RecordRow> rr;

    /*
    1. Отправляет запрос на сервер на получение списка записей
    2. сервер возвращает вектор записей
    */

    return rr;
}

// Открыть запись для чтения
void DepositDatabase::record(unsigned int id, Deposit &record) const {
    /*
    1. Отправляет запрос на сервер на получение записи по id
    2. сервер возвращает запись
    */
    Deposit::D d;

    record = Deposit::fromStruct(d);
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
    /*
    1. Отправляет запрос на сервер на получение кол-ва списка записей
    2. сервер возвращает кол-во записей
    */

    return 0;
}
