#include <QCoreApplication>
#include <depositDatabase.h>
#include <clocale>
#include <Windows.h>

const LPCTSTR SERVERPIPE = TEXT("\\\\.\\pipe\\bankserver");
DepositDatabase* dd;

DWORD WINAPI handleClient(HANDLE);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    setlocale(LC_ALL, "Russian");
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    dd = new DepositDatabase();

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

    //dd = new DepositDatabase(hServerPipe);

    // Цикл подключения клиентов
    while (true) {
        // Ожидание соединения
        if (!ConnectNamedPipe(hServerPipe, NULL)) {
            qDebug() << "Ошибка при ожидании подключения: " << GetLastError();
            CloseHandle(hServerPipe);
            return 0;
        }
        qDebug() << "Успешное подключение.\n";

        // Создание потока для обслуживания клиента
        CreateThread(NULL, 0, handleClient, hServerPipe, 0, NULL);

        QThread::sleep(1000);
        qDebug() << "Ожидание следующего клиента.\n";

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
    }

    QCoreApplication::quit();

    return a.exec();
}

DWORD WINAPI handleClient(HANDLE pipe) {
    HANDLE hClientPipe = (HANDLE)pipe;

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
        qDebug() << req;

        switch (req) {
        case APPEND_REQ:
            dd->append(hClientPipe);
            break;
        case REMOVE_REQ:
            dd->remove(hClientPipe);
            break;
        case SAVE_REQ:
            dd->save();
            break;
        case RECORD_REQ:
            dd->record(hClientPipe);
            break;
        case RECORDS_REQ:
            dd->records(hClientPipe);
            break;
        case COUNT_REQ:
            dd->count(hClientPipe);
            break;
        case UPDATE_REQ:
            dd->update(hClientPipe);
            break;
        }
    } while (req != FINISH_REQ);

    qDebug() << "Отключение успешно.\n";
    CloseHandle(hClientPipe);
}
