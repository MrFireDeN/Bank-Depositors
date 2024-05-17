#include <QCoreApplication>
#include <depositDatabase.h>
#include <clocale>
#include <Windows.h>

const LPCTSTR SERVERPIPE = TEXT("\\\\.\\pipe\\bankserver");
DepositDatabase* dd;

// Обслуживание клиента
DWORD WINAPI handleClient(HANDLE);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    setlocale(LC_ALL, "Russian");
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    dd = new DepositDatabase();

    // Создание именованного канала для приема подключений
    HANDLE hPipe = CreateNamedPipe(
        SERVERPIPE,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_BYTE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        1024,
        1024,
        5000,
        NULL
        );

    if (hPipe == INVALID_HANDLE_VALUE) {
        qDebug() << "Ошибка создания канала: " << GetLastError();
        return 0;
    }
    qDebug() << "Именованный канал успешно создан.\n";

    // Ожидание соединения
    if (!ConnectNamedPipe(hPipe, NULL)) {
        qDebug() << "Ошибка при ожидании подключения: " << GetLastError();
        CloseHandle(hPipe);
        return 0;
    }
    qDebug() << "Успешное подключение.\n";

    if (handleClient(hPipe)) {
        QCoreApplication::quit();
    }

    return a.exec();
}

DWORD WINAPI handleClient(HANDLE hPipe) {
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
        ReadFile(hPipe, (LPVOID)&req, sizeof(int), &bytesRead, NULL);
        qDebug() << req;

        switch (req) {

        // Выполнить добавление записи
        case APPEND_REQ:
            dd->append(hPipe);
            break;

        // Выполнить удаление записи
        case REMOVE_REQ:
            dd->remove(hPipe);
            break;

        // Выполнить сохранение записей
        case SAVE_REQ:
            dd->save();
            break;

        // Выпонить возвращение записи
        case RECORD_REQ:
            dd->record(hPipe);
            break;

        // Выполнить возвращение вектора записей
        case RECORDS_REQ:
            dd->records(hPipe);
            break;

        // Выполнить возвращение кол-во записей
        case COUNT_REQ:
            dd->count(hPipe);
            break;

        // Выполнить обновление записи
        case UPDATE_REQ:
            dd->update(hPipe);
            break;
        }
    } while (req != FINISH_REQ);

    qDebug() << "Отключение успешно.\n";
    CloseHandle(hPipe);
    return TRUE;
}
