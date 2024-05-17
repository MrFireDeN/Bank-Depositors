#include "depositDatabase.h"

WORD wVersionRequested;
WSADATA wsaData;
int err;

DepositDatabase::DepositDatabase()
{
}

// Подключение к серверу
bool DepositDatabase::connect() {
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        qDebug() << "Ошибка в WSAStartup: " << err;
        return 0;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in sain;
    sain.sin_family = AF_INET;
    sain.sin_port = htons(52000);
    sain.sin_addr.s_addr = inet_addr("127.0.0.1");

    ::connect(sock, (sockaddr *)&sain, sizeof(sain));
    qDebug() << sock;

    return 1;
}

// Отключение от серевера
bool DepositDatabase::disconnect() {
    req = FINISH_REQ;
    send(sock, (char*)&req, sizeof(req), 0);

    closesocket(sock);
    WSACleanup();

    return 1;
}

// Добавить элемент в базу данных
int DepositDatabase::append(Deposit& record) {
    req = APPEND_REQ;
    send(sock, (char*)&req, sizeof(req), 0);

    DepositCPY recordCPY = toStruct(record);

    send(sock, (char*)&recordCPY, sizeof(recordCPY), 0);

    recv(sock, (char*)&record.id, sizeof(record.id), 0);
    recv(sock, (char*)&pos, sizeof(pos), 0);

    return pos;
}

// Удаленить элемент из базы данных
void DepositDatabase::remove(unsigned int id) {
    req = REMOVE_REQ;
    send(sock, (char*)&req, sizeof(req), 0);

    send(sock, (char*)&id, sizeof(id), 0);
}

// Обновить запись в базе данных
int DepositDatabase::update(const Deposit& record) {
    // Создаем копию объекта record
    Deposit recordCopy = record;

    req = UPDATE_REQ;
    send(sock, (char*)&req, sizeof(req), 0);

    DepositCPY recordCPY = toStruct(recordCopy);
    send(sock, (char*)&recordCPY, sizeof(recordCPY), 0);

    recv(sock, (char*)&pos, sizeof(pos), 0);

    return pos;
}

// Возвратить вектор записей
const QVector<DepositDatabase::RecordRow> DepositDatabase::records() {
    QVector<RecordRow> rr;

    req = RECORDS_REQ;
    send(sock, (char*)&req, sizeof(req), 0);

    // Пример чтения из канала
    int count;
    recv(sock, (char*)&count, sizeof(count), 0);
    DepositCPY recordCPY;
    Deposit deposit;

    for (int i = 0; i < count; ++i) {
        recv(sock, (char*)&recordCPY.id, sizeof(recordCPY.id), 0);
        recv(sock, (char*)&recordCPY.accountNumber, sizeof(recordCPY.accountNumber), 0);
        recv(sock, (char*)&recordCPY.amount, sizeof(recordCPY.amount), 0);

        rr.append(toRecord(recordCPY));
    }

    return rr;
}

// Открыть запись для чтения
void DepositDatabase::record(unsigned int id, Deposit &record) {
    req = RECORD_REQ;
    send(sock, (char*)&req, sizeof(req), 0);

    send(sock, (char*)&id, sizeof(id), 0);

    DepositCPY recordCPY;

    recv(sock, (char*)&recordCPY, sizeof(recordCPY), 0);

    record = fromStruct(recordCPY);
}

// Сохранить файл на сервер
bool DepositDatabase::save() {
    req = SAVE_REQ;
    send(sock, (char*)&req, sizeof(req), 0);

    return 1;
}

// Загрузить файл с сервера
bool DepositDatabase::load() {
    return 1;
}

// Кол-во элементов
int DepositDatabase::count() {
    req = COUNT_REQ;
    send(sock, (char*)&req, sizeof(req), 0);

    int count;
    recv(sock, (char*)&count, sizeof(count), 0);

    return count;
}
