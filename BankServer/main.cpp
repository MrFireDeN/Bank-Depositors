#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <QCoreApplication>
#include <depositDatabase.h>
#include <clocale>
#include <Windows.h>
#include <ws2tcpip.h>

DepositDatabase* dd;

WORD wVersionRequested;
WSADATA wsaData;
int err;

// Обслуживание клиента
DWORD WINAPI handleClient(LPVOID);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    setlocale(LC_ALL, "Russian");
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        qDebug() << "Ошибка в WSAStartup: " << err;
        return 0;
    }

    dd = new DepositDatabase();

    SOCKET sock, kl_sock1;
    int size;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in sain;
    sain.sin_family = AF_INET;
    sain.sin_port = htons(52000);
    sain.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(sock, (sockaddr *)&sain, sizeof(sain)) == SOCKET_ERROR) {
        qDebug() << "Ошибка при привязке сокета: " << WSAGetLastError();
        closesocket(sock);
        WSACleanup();
        return 0;
    }
    qDebug() << "Сокет привязан успешно!";

    while (1)
    {
        listen(sock, 5);
        size = sizeof(sain);
        kl_sock1 = accept(sock, (sockaddr *)&sain, &size);
        if (kl_sock1 == INVALID_SOCKET) {
            qDebug() << "Ошибка при принятии соединения: " << WSAGetLastError();
            continue;
        }
        qDebug() << "Соединение принято успешно!";

        CreateThread(NULL, 0, handleClient, (LPVOID)(uintptr_t)kl_sock1, 0, NULL);
    }

    WSACleanup();

    return a.exec();
}

DWORD WINAPI handleClient(LPVOID lpParam) {
    SOCKET sock = (SOCKET)(uintptr_t)lpParam;

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

    do {
        // Загрузка номера запроса
        recv(sock, (char*)&req, sizeof(req), 0);

        switch (req) {

        // Выполнить добавление записи
        case APPEND_REQ:
            dd->append(sock);
            break;

        // Выполнить удаление записи
        case REMOVE_REQ:
            dd->remove(sock);
            break;

        // Выполнить сохранение записей
        case SAVE_REQ:
            dd->save();
            break;

        // Выпонить возвращение записи
        case RECORD_REQ:
            dd->record(sock);
            break;

        // Выполнить возвращение вектора записей
        case RECORDS_REQ:
            dd->records(sock);
            break;

        // Выполнить возвращение кол-во записей
        case COUNT_REQ:
            dd->count(sock);
            break;

        // Выполнить обновление записи
        case UPDATE_REQ:
            dd->update(sock);
            break;
        }
    } while (req != FINISH_REQ);

    qDebug() << "Отключение успешно.\n";
    closesocket(sock);
    return TRUE;
}
