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
    QRegularExpression reAccRu("^[А-Я][а-я]+\\s[А-Я][а-я]+\\s[А-Я][а-я]+$");
    QRegularExpression reAccEn("^[A-Z][a-z]+\\s[A-Z][a-z]+\\s[A-Z][a-z]+$");
    //QRegExp reIntRu("^([А-Я]?([а-я]+)?)?(\\s)?([А-Я]?([а-я]+)?)?(\\s)?([А-Я]?([а-я]+)?)?$");
    //QRegExp reIntEn("^([A-Z]?([a-z]+)?)?(\\s)?([A-Z]?([a-z]+)?)?(\\s)?([A-Z]?([a-z]+)?)?$");

    if (reAccRu.match(string).hasMatch() || reAccEn.match(string).hasMatch())
        return QValidator::Acceptable;
    //else if (reIntRu.exactMatch(string) || reIntEn.exactMatch(string))
    //    return QValidator::Intermediate;
    else
        return QValidator::Intermediate;
}
