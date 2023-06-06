#include "depositDatabase.h"

DepositDatabase::DepositDatabase()
{

}

// Добавить элемент
int DepositDatabase::add(const Deposit& record) {
    QVector<Deposit>::const_iterator i = database.constBegin();

    while (i != database.constEnd() && *i > record)
        ++i;

    if (i == database.constEnd()) {
        database.append(record);
        return database.count()-1;
    }

    int row = (i - database.constBegin());

    database.insert(i, record);

    return row;
}

// Добавить элемент в базу данных
int DepositDatabase::append(Deposit& record) {
    record.id = ++id;

    return add(record);
}

// Удаленить элемент из базы данных
void DepositDatabase::remove(unsigned int id) {
    QVector <Deposit>::const_iterator i;

    for (i = database.constBegin(); i != database.constEnd(); ++i) {
        if (i->id == id) {
            database.erase(i);
            return;
        }
    }
}

// Обновить запись в базе данных
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

// Открыть запись для чтения
void DepositDatabase::record(unsigned int id, Deposit &record) const {
    QVector<Deposit>::const_iterator i = database.constBegin();

    while (i != database.constEnd() && i->id != id)
        ++i;

    if (i == database.constEnd())
        return;

    record = *i;
}

// Сохранить файл базы данных
bool DepositDatabase::save(QString filename) {
    // провека названия
    if (filename.isEmpty() || database.empty())
        return false;

    QFile MyFile(filename);

    if (MyFile.open(QIODevice::WriteOnly)) {
        QDataStream output(&MyFile);
        QVector<Deposit>::iterator i;

        for (i = database.end()-1; i != database.begin(); --i) {
            output << *i;
        }
        output << *database.constBegin();

        MyFile.close();

        return true;
    }

    return false;
}

// Загрузить файл базы данных
bool DepositDatabase::load(QString filename) {
    // провека названия
    if (filename.isEmpty())
        return false;

    QFile MyFile(filename);
    if (MyFile.open(QIODevice::ReadOnly)) {
        database.clear();

        QDataStream output(&MyFile);
        Deposit d;

        while (!output.atEnd()) {
            output >> d;
            database.append(d);
        }

        MyFile.close();

        return true;
    }

    return false;
}

// Кол-во элементов
int DepositDatabase::count() {
    return database.count();
}

// Уничтожение всех данных
void DepositDatabase::clear() {
    database.clear();
}
