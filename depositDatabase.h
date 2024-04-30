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

    RecordRow toRecord(DepositCPY recordCPY) {
        RecordRow rr;

        rr.id = recordCPY.id;
        rr.amount = recordCPY.amount;
        rr.accountNumber = QString::fromStdString(recordCPY.accountNumber);

        return rr;
    }

    DepositCPY fromRecord(RecordRow rr) {
        DepositCPY recordCPY;

        recordCPY.id = rr.id;
        recordCPY.amount = rr.amount;

        QByteArray stringData = rr.accountNumber.toUtf8();
        std::copy(stringData.constBegin(), stringData.constBegin()+qMin(20, stringData.size()), recordCPY.accountNumber);
        recordCPY.accountNumber[qMin(20, stringData.size())] = '\0';

        return recordCPY;
    }

    static DepositCPY toStruct(Deposit &record) {
        DepositCPY recordCPY;
        QByteArray stringData;

        // id
        recordCPY.id = record.id;

        // Номер счета
        stringData = record.accountNumber.toUtf8();
        std::copy(stringData.constBegin(), stringData.constBegin()+qMin(20, stringData.size()), recordCPY.accountNumber);
        recordCPY.accountNumber[qMin(21, stringData.size())] = '\0';

        // ФИО
        stringData = record.FIO.toUtf8();
        std::copy(stringData.constBegin(), stringData.constBegin()+qMin(45, stringData.size()), recordCPY.FIO);
        recordCPY.FIO[qMin(45, stringData.size())] = '\0';

        // Тип вклада
        recordCPY.type = record.type;

        // Дата рождения
        stringData = record.birthDate.toString("dd.MM.yyyy").toUtf8();
        std::copy(stringData.constBegin(), stringData.constBegin()+qMin(10, stringData.size()), recordCPY.birthDate);
        recordCPY.birthDate[qMin(11, stringData.size())] = '\0';

        // Сумма вклада
        recordCPY.amount = record.amount;

        // Процент вклада
        recordCPY.interest = record.interest;

        // Переодичность начисления
        recordCPY.accrualFrequency = record.accrualFrequency;

        // Последняя транзакция
        stringData = record.lastTransaction.toString("dd.MM.yyyy").toUtf8();
        std::copy(stringData.constBegin(), stringData.constBegin()+qMin(10, stringData.size()), recordCPY.lastTransaction);
        recordCPY.lastTransaction[qMin(11, stringData.size())] = '\0';

        // Наличие пластиковой карты
        recordCPY.plasticCardAvailability = record.plasticCardAvailability;

        return recordCPY;
    }

    static Deposit fromStruct(DepositCPY &recordCPY) {
        Deposit record;

        // id
        record.id = recordCPY.id;

        // Номер счета
        record.accountNumber = QString::fromStdString(recordCPY.accountNumber);

        // ФИО
        record.FIO = QString::fromStdString(recordCPY.FIO);

        // Тип вклада
        record.type = recordCPY.type;

        // Дата рождения
        record.birthDate = QDate::fromString(QString::fromStdString(recordCPY.birthDate), "dd.MM.yyyy");

        // Сумма вклада
        record.amount = recordCPY.amount;

        // Процент вклада
        record.interest = recordCPY.interest;

        // Переодичность начисления
        record.accrualFrequency = recordCPY.accrualFrequency;

        // Последняя транзакция
        record.lastTransaction = QDate::fromString(QString::fromStdString(recordCPY.lastTransaction), "dd.MM.yyyy");

        // Наличие пластиковой карты
        record.plasticCardAvailability = recordCPY.plasticCardAvailability;

        return record;
    }
};

#endif // RECORD_H
