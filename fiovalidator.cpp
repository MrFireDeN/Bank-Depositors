#include "fiovalidator.h"

FIOValidator::FIOValidator(QObject* parent)
    : QValidator (parent)
{

}

FIOValidator::~FIOValidator()
{

}

QValidator::State FIOValidator::validate(QString &string, int &pos) const
{
    QRegExp reAccRu("^[А-Я][а-я]+\\s[А-Я][а-я]+\\s[А-Я][а-я]+$");
    QRegExp reAccEn("^[A-Z][a-z]+\\s[A-Z][a-z]+\\s[A-Z][a-z]+$");
    QRegExp reIntRu("^([А-Я]?([а-я]+)?)?(\\s)?([А-Я]?([а-я]+)?)?(\\s)?([А-Я]?([а-я]+)?)?$");
    QRegExp reIntEn("^([A-Z]?([a-z]+)?)?(\\s)?([A-Z]?([a-z]+)?)?(\\s)?([A-Z]?([a-z]+)?)?$");

    if (reAccRu.exactMatch(string) || reAccEn.exactMatch(string))
        return QValidator::Acceptable;
    else if (reIntRu.exactMatch(string) || reIntEn.exactMatch(string))
        return QValidator::Intermediate;
    else
        return QValidator::Invalid;
}
