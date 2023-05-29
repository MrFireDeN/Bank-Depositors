#ifndef RANDOMRECORD_H
#define RANDOMRECORD_H

#include <QStringList>
#include "deposit.h"

class RandomRecord
{
public:
    RandomRecord();
    void setRecord(Deposit&);

private:
    QStringList F;
    QStringList I;
    QStringList O;

    void setFIO(Deposit&);
};

#endif // RANDOMRECORD_H
