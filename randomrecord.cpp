#include "randomrecord.h"

RandomRecord::RandomRecord()
{
    F << "Алексеев"
      <<  "Ананазаров"
      <<  "Атаджанов"
      <<  "Березовский"
      <<  "Кирюхин"
      <<  "Османов"
      <<  "Подмосковнов"
      <<  "Савушкин"
      <<  "Сагалаев"
      <<  "Свиридов"
      <<  "Соболь"
      <<  "Сытников"
      <<  "Чекашов"
      <<  "Чупрына"
      <<  "Шилин"
      <<  "Яшин";

    I << "Андрей"
      <<  "Аман"
      <<  "Мекан"
      <<  "Пётр"
      <<  "Матвей"
      <<  "Абдулрахман"
      <<  "Илья"
      <<  "Александр"
      <<  "Михаил"
      <<  "Денис"
      <<  "Павел"
      <<  "Олег"
      <<  "Матвей"
      <<  "Даниил"
      <<  "Арсений"
      <<  "Никита";

    O << "Леонидович"
      <<  "Реджепмурадович"
      <<  "Иванович"
      <<  "Валерьевич"
      <<  "Даньяевич"
      <<  "Юрьевич"
      <<  "Дмитриевич"
      <<  "Сергеевич"
      <<  "Евгеньевич"
      <<  "Александрович"
      <<  "Николаевич"
      <<  "Павлович"
      <<  "Васильевич"
      <<  "Игоревич"
      <<  "Денисович";
}

void RandomRecord::setFIO(Deposit& deposit) {
    deposit.FIO.clear();
    deposit.FIO.append(F[rand() % F.count()]);
    deposit.FIO.append(" ");
    deposit.FIO.append(I[rand() % I.count()]);
    deposit.FIO.append(" ");
    deposit.FIO.append(O[rand() % O.count()]);
}

void RandomRecord::setRecord(Deposit& deposit) {
    setFIO(deposit);

    deposit.accountNumber.clear();
    for (int i = 0; i < 20; i++)
        deposit.accountNumber.append(QString::number(rand() % 10));

    deposit.accrualFrequency = rand() % 4;
    deposit.amount = rand() % 100000  + ((float) (rand() % 100)) / 100;
    deposit.birthDate.setDate(QDate::currentDate().addYears(-100 + rand() % 80).year(), rand() % 12 + 1, rand() % 30);
    deposit.interest = rand() % 100 + ((float) (rand() % 100)) / 100;
    deposit.lastTransaction = QDate::currentDate();
    deposit.type = rand() % 3;
    deposit.plasticCardAvailability = rand() % 2;
}
