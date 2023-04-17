#include "deposit.h"

Deposit::Deposit() {
    accountNumber = "01234567891011121314"; // Номер счета
    type = 0; // Тип вклада
    // Инициализация остальных полей
    FIO = "Свиридов Денис Евгеньевич"; // ФИО
    birthDate.setDate(2002, 1, 26); // Дата рождения
    amount = 327498127491; // Сумма вклада
    interest = 15.9; // Процент вклада
    accrualFrequency = 0; // Переодичность начисления
    lastTransaction = QDate::currentDate().addDays(-1); // Последняя транзакция
    plasticCardAvailability = true; // Наличие пластиковой карты
}
