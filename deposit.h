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
};

#endif // DEPOSIT_H
