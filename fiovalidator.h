#ifndef FIOVALIDATOR_H
#define FIOVALIDATOR_H

#include <QValidator>

class FIOValidator : public QValidator
{
    Q_OBJECT
public:
    FIOValidator(QObject *parent);
    ~FIOValidator();

    QValidator::State validate(QString & string, int & pos) const;
};

#endif // FIOVALIDATOR_H
