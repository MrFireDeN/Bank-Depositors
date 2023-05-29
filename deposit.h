#ifndef DEPOSIT_H
#define DEPOSIT_H
#include <QString>
#include <QDate>

class Deposit
{
public:
    Deposit(); // Конструктор класса

    QString accountNumber; // Номер счета
    int type; // Тип вклада
    QString FIO; // ФИО
    QDate birthDate; // Дата рождения
    double amount; // Сумма вклада
    double interest; // Процент вклада
    int accrualFrequency; // Переодичность начисления
    QDate lastTransaction; // Последняя транзакция
    bool plasticCardAvailability; // Наличие пластиковой карты

    struct depositComparator
    {
        bool operator()(const Deposit& obj1, const Deposit& obj2) const {
            if (obj1.type < obj2.type) {
                return true;
            }
            if (obj1.type > obj2.type) {
                return false;
            }

            if (obj1.birthDate < obj2.birthDate)
                return true;
            if (obj1.birthDate > obj2.birthDate)
                return false;

            return obj1.FIO < obj2.FIO;
        }
    };
};

#endif // DEPOSIT_H
