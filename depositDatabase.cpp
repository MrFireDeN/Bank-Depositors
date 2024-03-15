#include "depositDatabase.h"

DepositDatabase::DepositDatabase()
{

}

// Добавить элемент
int DepositDatabase::add(const Deposit& record) {
//    QVector<Deposit>::const_iterator i = database.constBegin();

//    while (i != database.constEnd() && *i > record)
//        ++i;

//    if (i == database.constEnd()) {
//        database.append(record);
//        return database.count()-1;
//    }

//    int row = (i - database.constBegin());

//    database.insert(i, record);

//    return row;

    return 0;
}

// Добавить элемент в базу данных
int DepositDatabase::append(Deposit& record) {
    record.id = ++id;

    return add(record);
}

// Удаленить элемент из базы данных
void DepositDatabase::remove(unsigned int id) {
//    QVector <Deposit>::const_iterator i;

//    for (i = database.constBegin(); i != database.constEnd(); ++i) {
//        if (i->id == id) {
//            database.erase(i);
//            return;
//        }
//    }
}

// Обновить запись в базе данных
int DepositDatabase::update(const Deposit& record) {
    remove(record.id);

    return add(record);
}

// Возвратить вектор записей
const QVector<DepositDatabase::RecordRow> DepositDatabase::records() const {
    QVector<RecordRow> rr;

//    QVector<Deposit>::const_iterator i;

//    for (i = database.constBegin(); i != database.constEnd(); ++i)
//        rr.append({
//            i->id,
//            i->accountNumber,
//            i->amount
//        });

    return rr;
}

// Открыть запись для чтения
void DepositDatabase::record(unsigned int id, Deposit &record) const {
//    QVector<Deposit>::const_iterator i = database.constBegin();

//    while (i != database.constEnd() && i->id != id)
//        ++i;

//    if (i == database.constEnd())
//        return;

//    record = *i;
}

// Сохранить файл базы данных
bool DepositDatabase::save() {
    return true;
}

// Загрузить файл базы данных
bool DepositDatabase::load() {

//    HANDLE hPipe = CreateNamedPipe(
//        "\\\\.\\pipe\\myfirstpipe",
//        PIPE_ACCESS_DUPLEX,
//        PIPE_TYPE_MESSAGE |
//        PIPE_READMODE_BYTE | PIPE_WAIT,
//        PIPE_UNLIMITED_INSTANCES, 1024,
//        1024, 5000, NULL);

    return true;
}

// Кол-во элементов
int DepositDatabase::count() {
    //return database.count();

    return 0;
}

// Уничтожение всех данных
void DepositDatabase::clear() {
    //database.clear();
}
