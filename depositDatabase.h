#ifndef RECORD_H
#define RECORD_H

#include <QVector>
#include <QFile>
#include "deposit.h"
#include <QThread>
#include <QDate>
#include <QDataStream>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

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

    int count();
    int append(Deposit &record);
    void remove(unsigned int id);
    int update(const Deposit& record);
    void record(unsigned int id, Deposit& record);
    const QVector<RecordRow> records();
    bool save();
    bool load();
    bool connect();
    bool disconnect();
    bool isModified() const;

private:
    const LPCTSTR SERVERNAME = TEXT("server\\bankserver.exe");
    const LPCTSTR SERVERPIPE = TEXT("\\\\.\\pipe\\bankserver");

    const DWORD
        FINISH_REQ  = 0,
        APPEND_REQ  = 1,
        REMOVE_REQ  = 2,
        SAVE_REQ    = 3,
        RECORD_REQ  = 4,
        RECORDS_REQ = 5,
        COUNT_REQ   = 6,
        UPDATE_REQ  = 7;

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    HANDLE hPipe;
    DWORD mode, bytesWritten, bytesRead;
    char buffer[1024];
    int bufferSize = 1024;
    int req, pos;

    unsigned int id;

    RecordRow toRecord(Deposit::D);
    Deposit::D fromRecord(RecordRow);
};

#endif // RECORD_H
