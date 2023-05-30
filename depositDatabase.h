#ifndef RECORD_H
#define RECORD_H

#include <QMap>
#include "deposit.h"

class DepositDatabase
{
public:
    DepositDatabase();

    int count();
    int append(Deposit &record);
    void remove(unsigned int id);
    int update(const Deposit& record);
    void showRecord(unsigned int id, Deposit& record) const;
    const QVector<int/*класс*/> records() const;
    bool save(QString filename);
    bool load(QString filename);
    void clear();
    bool isModified() const;

private:
    QVector<Deposit> record;

};

#endif // RECORD_H
