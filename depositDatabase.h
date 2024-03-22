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

    int count();
    int append(Deposit &record);
    void remove(unsigned int id);
    int update(const Deposit& record);
    void record(unsigned int id, Deposit& record) const;
    const QVector<RecordRow> records() const;
    bool save();
    bool load();
    bool connect();
    bool disconnect();
    void clear();
    bool isModified() const;

private:
    const LPCTSTR SERVERNAME = TEXT("bankserver.exe");
    const LPCTSTR SERVERPIPE = TEXT("\\\\.\\pipe\\bankserver");

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    HANDLE hPipe;
    DWORD mode;
    int add(const Deposit& record);

    unsigned int id;
};

#endif // RECORD_H
