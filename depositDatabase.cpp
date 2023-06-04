#include "depositDatabase.h"

DepositDatabase::DepositDatabase()
{

}

int DepositDatabase::add(const Deposit& record) {
    QVector<Deposit>::const_iterator i = database.constBegin();

    while(i != database.constEnd() &&
           i->type <= record.type)
        ++i;

    while(i != database.constEnd() &&
           (i+1)->type == record.type &&
           i->FIO <= record.FIO)
        ++i;

    while(i != database.constEnd() &&
           (i+1)->type == record.type &&
           (i+1)->FIO == record.FIO &&
           i->birthDate <= record.birthDate)
        ++i;

    if (i == database.constEnd()) {
        database.append(record);
        return database.count()-1;
    }

    int row = (i - database.constBegin());

    database.insert(i, record);

//    if (i == database.constBegin())
//        return 0;


    return row;
}

// Добавление элемента
int DepositDatabase::append(Deposit& record) {
    record.id = ++id;

    return add(record);
}

// Удаление элементов
void DepositDatabase::remove(unsigned int id) {
    QVector <Deposit>::const_iterator i;

    for (i = database.constBegin(); i != database.constEnd(); ++i) {
        if (i->id == id) {
            database.erase(i);
            return;
        }
    }
}

// Изменить запись в базе данных
int DepositDatabase::update(const Deposit& record) {
    remove(record.id);

    return add(record);
}

// Возвратить вектор записей
const QVector<DepositDatabase::RecordRow> DepositDatabase::records() const {
    QVector<RecordRow> rr;

    QVector<Deposit>::const_iterator i;

    for (i = database.constBegin(); i != database.constEnd(); ++i)
        rr.append({
            i->id,
            i->accountNumber,
            i->amount
        });

    return rr;
}

// Запись для чтения
void DepositDatabase::record(unsigned int id, Deposit &record) const {
    QVector<Deposit>::const_iterator i = database.constBegin();

    while (i != database.constEnd() && i->id != id)
        ++i;

    if (i == database.constEnd())
        return;

    record = *i;
}

// Кол-во элементов
int DepositDatabase::count() {
    return database.count();
}

// Уничтожение всех данных
void DepositDatabase::clear() {
    database.clear();
}
