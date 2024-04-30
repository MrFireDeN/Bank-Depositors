#ifndef RECORD_H
#define RECORD_H

#include <QVector>
#include <QFile>
#include "deposit.h"
#include <QDate>
#include <QDataStream>
#include <Windows.h>
#include <QThread>

class DepositDatabase
{
public:
    DepositDatabase();
    ~DepositDatabase();

    struct RecordRow
    {
        unsigned int id; // идентификатор
        QString accountNumber; // номер счета
        double amount; // сумма счета
    };

    bool append(HANDLE);
    bool remove(HANDLE);
    bool update(HANDLE);
    bool load();
    bool save();
    bool record(HANDLE);
    bool records(HANDLE);
    bool count(HANDLE);
    void clear();
    bool isModified() const;

private:
    QVector<Deposit> database;
    int add(const Deposit& record);

    const LPCTSTR FILENAME = TEXT("main.dd");
    const LPCTSTR SERVERNAME = TEXT("bankserver.exe");

    DWORD mode, bytesWritten, bytesRead;
    char buffer[1024];
    int bufferSize = 1024;
    int req, pos;

    unsigned int id = 0;

    static DepositCPY toStruct(Deposit &record){
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
