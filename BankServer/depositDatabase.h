#ifndef RECORD_H
#define RECORD_H

#include <QVector>
#include <QFile>
#include "deposit.h"
#include <QDate>
#include <QDataStream>
#include <Windows.h>

class DepositDatabase
{
public:
    DepositDatabase();

    struct RecordRow
    {
        unsigned int id; // идентификатор
        QString accountNumber; // номер счета
        double amount; // сумма счета
    };

    bool start();
    bool append();
    bool remove();
    bool send();
    bool load();
    bool record() const;
    bool records() const;
    bool count();
    void clear();
    bool isModified() const;

private:
    QVector<Deposit> database;
    int add(const Deposit& record);

    const LPCTSTR FILENAME = TEXT("main.dd");
    const LPCTSTR SERVERNAME = TEXT("bankserver.exe");
    const LPCTSTR SERVERPIPE = TEXT("\\\\.\\pipe\\bankserver");

    HANDLE hPipe;
    DWORD mode, bytesWritten, bytesRead;
    int req, pos;

    unsigned int id;
};

#endif // RECORD_H
