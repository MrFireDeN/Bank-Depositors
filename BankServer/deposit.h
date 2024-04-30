#ifndef DEPOSIT_H
#define DEPOSIT_H
#include <QString>
#include <QDate>
#include <QFile>
#include <QDataStream>

struct DepositCPY {
    unsigned int id;                // id
    char accountNumber[21];         // Номер счета
    int type;                       // Тип вклада
    char FIO[45];                   // ФИО
    char birthDate[11];             // Дата рождения
    double amount;                  // Сумма вклада
    double interest;                // Процент вклада
    int accrualFrequency;           // Переодичность начисления
    char lastTransaction[11];       // Последняя транзакция
    bool plasticCardAvailability;   // Наличие пластиковой карты
};

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
};

#endif // DEPOSIT_H
