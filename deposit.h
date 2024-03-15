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
        char accountNumber[21];         // Номер счета
        int type;                       // Тип вклада
        char FIO[46];                   // ФИО
        char birthDate[11];             // Дата рождения
        double amount;                  // Сумма вклада
        double interest;                // Процент вклада
        int accrualFrequency;           // Переодичность начисления
        char lastTransaction[11];       // Последняя транзакция
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

    static D toStruct(Deposit &deposit) {
        D d;

        // id
        d.id = deposit.id;

        // Номер счета
        std::strcpy(d.accountNumber, deposit.accountNumber.toStdString().c_str());
        d.accountNumber[sizeof(d.accountNumber)-1] = '\0';

        // ФИО
        std::strcpy(d.FIO, deposit.FIO.toStdString().c_str());
        d.FIO[sizeof(d.FIO)-1] = '\0';

        // Тип вклада
        d.type = deposit.type;

        // Дата рождения
        std::strcpy(d.birthDate, deposit.birthDate.toString().toStdString().c_str());
        d.birthDate[sizeof(d.birthDate)-1] = '\0';

        // Сумма вклада
        d.amount = deposit.amount;

        // Процент вклада
        d.interest = deposit.interest;

        // Переодичность начисления
        d.accrualFrequency = deposit.accrualFrequency;

        // Последняя транзакция
        std::strcpy(d.lastTransaction, deposit.lastTransaction.toString().toStdString().c_str());
        d.lastTransaction[sizeof(d.lastTransaction)-1] = '\0';

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
        deposit.birthDate = QDate::fromString(QString::fromStdString(d.birthDate));

        // Сумма вклада
        deposit.amount = d.amount;

        // Процент вклада
        deposit.interest = d.interest;

        // Переодичность начисления
        deposit.accrualFrequency = d.accrualFrequency;

        // Последняя транзакция
        deposit.lastTransaction = QDate::fromString(QString::fromStdString(d.lastTransaction));

        // Наличие пластиковой карты
        deposit.plasticCardAvailability = d.plasticCardAvailability;

        return deposit;
    }
};

#endif // DEPOSIT_H
