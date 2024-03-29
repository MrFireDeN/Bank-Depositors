#ifndef DEPOSIT_H
#define DEPOSIT_H
#include <QString>
#include <QDate>
#include <QFile>
#include <QDataStream>

class Deposit
{

public:
    Deposit(); // Конструктор класса

    unsigned int id;                // id
    QString accountNumber;          // Номер счета
    int type;                       // Тип вклада
    QString FIO;                    // ФИО
    QDate birthDate;                // Дата рождения
    double amount;                  // Сумма вклада
    double interest;                // Процент вклада
    int accrualFrequency;           // Переодичность начисления
    QDate lastTransaction;          // Последняя транзакция
    bool plasticCardAvailability;   // Наличие пластиковой карты


    struct D {
        unsigned int id;                // id
        char accountNumber[20];         // Номер счета
        int type;                       // Тип вклада
        char FIO[45];                   // ФИО
        char birthDate[10];             // Дата рождения
        double amount;                  // Сумма вклада
        double interest;                // Процент вклада
        int accrualFrequency;           // Переодичность начисления
        char lastTransaction[10];       // Последняя транзакция
        bool plasticCardAvailability;   // Наличие пластиковой карты
    };

    bool operator>(const Deposit& d) const {
        if (d.type != type)
            return d.type > type;

        if (d.birthDate != birthDate)
            return d.birthDate > birthDate;

        return d.FIO >= FIO;
    }

    bool operator==(Deposit& d) {
        return id == d.id;
    }

    bool operator!=(Deposit& d) {
        return id != d.id;
    }

    friend QDataStream & operator<<(QDataStream & out, const Deposit & d) {
        out << d.id
            << d.accountNumber
            << d.type
            << d.FIO
            << d.birthDate
            << d.amount
            << d.interest
            << d.accrualFrequency
            << d.lastTransaction
            << d.plasticCardAvailability;

        return out;
    }

    friend QDataStream & operator>>(QDataStream & in, Deposit & d) {
        in  >> d.id
            >> d.accountNumber
            >> d.type
            >> d.FIO
            >> d.birthDate
            >> d.amount
            >> d.interest
            >> d.accrualFrequency
            >> d.lastTransaction
            >> d.plasticCardAvailability;

        return in;
    }

    static D toStruct(Deposit &deposit){
        D d;
        QByteArray stringData;

        // id
        d.id = deposit.id;

        // Номер счета
        stringData = deposit.accountNumber.toUtf8();
        std::copy(stringData.constBegin(), stringData.constBegin()+qMin(20, stringData.size()), d.accountNumber);


        // ФИО
        stringData = deposit.FIO.toUtf8();
        std::copy(stringData.constBegin(), stringData.constBegin()+qMin(45, stringData.size()), d.FIO);
        d.FIO[qMin(45, stringData.size())] = '\0';

        // Тип вклада
        d.type = deposit.type;

        // Дата рождения
        stringData = deposit.birthDate.toString("dd.MM.yyyy").toUtf8();
        std::copy(stringData.constBegin(), stringData.constBegin()+qMin(10, stringData.size()), d.birthDate);

        // Сумма вклада
        d.amount = deposit.amount;

        // Процент вклада
        d.interest = deposit.interest;

        // Переодичность начисления
        d.accrualFrequency = deposit.accrualFrequency;

        // Последняя транзакция
        stringData = deposit.lastTransaction.toString("dd.MM.yyyy").toUtf8();
        std::copy(stringData.constBegin(), stringData.constBegin()+qMin(10, stringData.size()), d.lastTransaction);

        // Наличие пластиковой карты
        d.plasticCardAvailability = deposit.plasticCardAvailability;

        return d;
    }

    static Deposit fromStruct(D &d) {
        Deposit deposit;

        // id
        deposit.id = d.id;

        // Номер счета
        deposit.accountNumber = QString::fromStdString(d.accountNumber);

        // ФИО
        deposit.FIO = QString::fromStdString(d.FIO);

        // Тип вклада
        deposit.type = d.type;

        // Дата рождения
        deposit.birthDate = QDate::fromString(QString::fromStdString(d.birthDate), "dd.MM.yyyy");

        // Сумма вклада
        deposit.amount = d.amount;

        // Процент вклада
        deposit.interest = d.interest;

        // Переодичность начисления
        deposit.accrualFrequency = d.accrualFrequency;

        // Последняя транзакция
        deposit.lastTransaction = QDate::fromString(QString::fromStdString(d.lastTransaction), "dd.MM.yyyy");

        // Наличие пластиковой карты
        deposit.plasticCardAvailability = d.plasticCardAvailability;

        return deposit;
    }
};

#endif // DEPOSIT_H
