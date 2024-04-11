#ifndef RECORD_H
#define RECORD_H

#include <QVector>
#include <QFile>
#include "deposit.h"
#include <QDate>
#include <QDataStream>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <QThread>

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
    bool update();
    bool load();
    bool save();
    bool record();
    bool records();
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
    char buffer[1024];
    int bufferSize = 1024;
    int req, pos;

    unsigned int id = 0;

    RecordRow toRecord(Deposit::D);
    Deposit::D fromRecord(RecordRow);

    static DWORD WINAPI handleClient(LPVOID);
    void handleClientLogic(LPVOID);
};

#endif // RECORD_H
