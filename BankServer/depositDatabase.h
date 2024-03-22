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
    int count();
    int append(Deposit &record);
    void remove(unsigned int id);
    int update(const Deposit& record);
    void record(unsigned int id, Deposit& record) const;
    const QVector<RecordRow> records() const;
    bool send();
    bool load();
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
