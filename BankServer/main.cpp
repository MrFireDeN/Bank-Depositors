#include <QCoreApplication>
#include <depositDatabase.h>
#include <clocale>
#include <Windows.h>

const LPCTSTR SERVERPIPE = TEXT("\\\\.\\pipe\\bankserver");
DepositDatabase* dd;

// Констуктор функий
DWORD WINAPI handleClient(HANDLE);
BOOL WINAPI ConsoleCtrlHandler(DWORD);
DWORD WINAPI saveDataPeriodically(LPVOID);

HANDLE hSaveThread;

// Создание критических секций
CRITICAL_SECTION TOTAL_BLOCK;
CRITICAL_SECTION LOCAL_BLOCK;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    setlocale(LC_ALL, "Russian");
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    dd = new DepositDatabase();

    // Инициализировать объект критической секции
    InitializeCriticalSection(&TOTAL_BLOCK);
    InitializeCriticalSection(&LOCAL_BLOCK);

    // Установка обработчика событий консоли
    SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);

    // Создание потока для периодического сохранения данных
    hSaveThread = CreateThread(NULL, 0, saveDataPeriodically, NULL, 0, NULL);

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

        // Создание потока для обслуживания клиента
        CreateThread(NULL, 0, handleClient, hServerPipe, 0, NULL);

        QThread::sleep(1);
        qDebug() << "Ожидание следующего клиента.\n";

        // Создание именованного канала для приема подключений
        hServerPipe = CreateNamedPipe(
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
            // Установить полную блокировку
            EnterCriticalSection(&TOTAL_BLOCK);

            // Выполнить добавление записи
            dd->append(hClientPipe);
            Sleep(1000);

            // Снять полную блокировку
            LeaveCriticalSection(&TOTAL_BLOCK);
            break;
        case REMOVE_REQ:
            // Установить полную блокировку
            EnterCriticalSection(&TOTAL_BLOCK);

            // Выполнить удаление записи
            dd->remove(hClientPipe);
            Sleep(1000);

            // Снять полную блокировку
            LeaveCriticalSection(&TOTAL_BLOCK);
            break;
        case SAVE_REQ:
            // Установить полную блокировку
            EnterCriticalSection(&TOTAL_BLOCK);
            // Установить частичную блокировку
            EnterCriticalSection(&LOCAL_BLOCK);
            // Снять полную блокировку
            LeaveCriticalSection(&TOTAL_BLOCK);

            // Выполнить сохранение записей
            dd->save();
            Sleep(1000);

            // Снять частичную блокировку
            LeaveCriticalSection(&LOCAL_BLOCK);
            break;
        case RECORD_REQ:
            // Установить полную блокировку
            EnterCriticalSection(&TOTAL_BLOCK);
            // Установить частичную блокировку
            EnterCriticalSection(&LOCAL_BLOCK);
            // Снять полную блокировку
            LeaveCriticalSection(&TOTAL_BLOCK);

            // Выпонить возвращение записи
            dd->record(hClientPipe);
            Sleep(1000);

            // Снять частичную блокировку
            LeaveCriticalSection(&LOCAL_BLOCK);
            break;
        case RECORDS_REQ:
            // Установить полную блокировку
            EnterCriticalSection(&TOTAL_BLOCK);
            // Установить частичную блокировку
            EnterCriticalSection(&LOCAL_BLOCK);
            // Снять полную блокировку
            LeaveCriticalSection(&TOTAL_BLOCK);

            // Выполнить возвращение вектора записей
            dd->records(hClientPipe);
            Sleep(1000);

            // Снять частичную блокировку
            LeaveCriticalSection(&LOCAL_BLOCK);
            break;
        case COUNT_REQ:
            // Установить полную блокировку
            EnterCriticalSection(&TOTAL_BLOCK);
            // Установить частичную блокировку
            EnterCriticalSection(&LOCAL_BLOCK);
            // Снять полную блокировку
            LeaveCriticalSection(&TOTAL_BLOCK);

            // Выполнить возвращение кол-во записей
            dd->count(hClientPipe);
            Sleep(1000);

            // Снять частичную блокировку
            LeaveCriticalSection(&LOCAL_BLOCK);
            break;
        case UPDATE_REQ:
            // Установить полную блокировку
            EnterCriticalSection(&TOTAL_BLOCK);

            // Выполнить обновление записи
            dd->update(hClientPipe);
            Sleep(1000);

            // Снять полную блокировку
            LeaveCriticalSection(&TOTAL_BLOCK);
            break;
        }
    } while (req != FINISH_REQ);

    qDebug() << "Отключение успешно.\n";
    CloseHandle(hClientPipe);
}

BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType) {
    switch (dwCtrlType) {
    case CTRL_CLOSE_EVENT:
        // Установить полную блокировку
        EnterCriticalSection(&TOTAL_BLOCK);

        delete(dd);

        // Снять полную блокировку
        LeaveCriticalSection(&TOTAL_BLOCK);

        // Удалить объект критической секции
        DeleteCriticalSection(&TOTAL_BLOCK);
        DeleteCriticalSection(&LOCAL_BLOCK);

        // Ожидание завершения работы потока
        WaitForSingleObject(hSaveThread, INFINITE);
        CloseHandle(hSaveThread);
        return TRUE;
    default:
        return FALSE;
    }
}

DWORD WINAPI saveDataPeriodically(LPVOID lpParam) {
    while (true) {
        // Установить полную блокировку
        EnterCriticalSection(&TOTAL_BLOCK);
        // Установить частичную блокировку
        EnterCriticalSection(&LOCAL_BLOCK);
        // Снять полную блокировку
        LeaveCriticalSection(&TOTAL_BLOCK);

        // Сохранение данных
        dd->save();
        qDebug() << "Данные сохранены.";

        // Снять частичную блокировку
        LeaveCriticalSection(&LOCAL_BLOCK);

        // Подождать 15 секунд
        Sleep(15000); // 15 секунд в миллисекундах
    }

    return 0;
}
