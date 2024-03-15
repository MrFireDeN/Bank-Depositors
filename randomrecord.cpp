#include "randomrecord.h"

RandomRecord::RandomRecord()
{
    F << "Alexeev"
      << "Berezovsky"
      << "Kiryukhin"
      << "Osmanov"
      << "Podmoskovnov"
      << "Savushkin"
      << "Sagalaev"
      << "Sviridov"
      << "Sobol"
      << "Sytnikov"
      << "Chekashov"
      << "Chupryna"
      << "Shilin";

    I << "Andrey"
      << "Peter"
      << "Matvey"
      << "Abdulrahman"
      << "Ilya"
      << "Alexander"
      << "Mikhail"
      << "Denis"
      << "Pavel"
      << "Oleg"
      << "Matvey"
      << "Daniil"
      << "Arseny";

    O << "Leonidovich"
      << "Ivanovich"
      << "Valeryevich"
      << "Danyayevich"
      << "Yuryevich"
      << "Dmitrievich"
      << "Sergeevich"
      << "Yevgenevich"
      << "Alexandrovich"
      << "Nikolaevich"
      << "Pavlovich"
      << "Vasilyevich"
      << "Igoryevich";
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
