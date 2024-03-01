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
bool DepositDatabase::save() {
    // Создание файла
    HANDLE myFile = CreateFile(
        FILENAME,               // Имя файла
        GENERIC_READ,           // Желаемый доступ к файлу (здесь только чтение)
        FILE_SHARE_READ,        // Режим разделения (можно открывать другим процессам для чтения)
        NULL,                   // Атрибуты безопасности (не используется)
        OPEN_EXISTING,          // Режим открытия (открываем существующий файл)
        FILE_ATTRIBUTE_NORMAL,  // Атрибуты файла (обычный файл)
        NULL                    // Дескриптор файла-шаблона (не используется)
        );

    // Проверка на успешность открытия файла
    if (myFile == INVALID_HANDLE_VALUE)
        return false;

    // Проверка базы дпнных на заполненность
    if (database.empty()){
        CloseHandle(myFile);
        return false;
    }

    QVector<Deposit>::iterator i;
    Deposit::D d;

    for (i = database.end()-1; i != database.begin(); --i) {
        d = Deposit::depositToStruct(*i);

        // id
        WriteFile(myFile, &d.id, sizeof(d.id), NULL, NULL);
        // Номер счета
        WriteFile(myFile, &d.accountNumber, sizeof(d.accountNumber), NULL, NULL);
        // Тип вклада
        WriteFile(myFile, &d.type, sizeof(d.type), NULL, NULL);
        // ФИО
        WriteFile(myFile, &d.FIO, sizeof(d.FIO), NULL, NULL);
        // Дата рождения
        WriteFile(myFile, &d.birthDate, sizeof(d.birthDate), NULL, NULL);
        // Сумма вклада
        WriteFile(myFile, &d.amount, sizeof(d.amount), NULL, NULL);
        // Процент вклада
        WriteFile(myFile, &d.interest, sizeof(d.interest), NULL, NULL);
        // Переодичность начисления
        WriteFile(myFile, &d.accrualFrequency, sizeof(d.accrualFrequency), NULL, NULL);
        // Последняя транзакция
        WriteFile(myFile, &d.lastTransaction, sizeof(d.lastTransaction), NULL, NULL);
        // Наличие пластиковой карты
        WriteFile(myFile, &d.plasticCardAvailability, sizeof(d.plasticCardAvailability), NULL, NULL);
    }

    CloseHandle(myFile);

    return true;

    /*
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
    */
}

// Загрузить файл базы данных
bool DepositDatabase::load() {
    // Создание файла
    HANDLE myFile = CreateFile(
        FILENAME,               // Имя файла
        GENERIC_WRITE,          // Желаемый доступ к файлу (здесь только запись)
        0,                      // Режим разделения (нельзя открывать другим процессам)
        NULL,                   // Атрибуты безопасности (не используется)
        CREATE_ALWAYS,          // Режим создания (создаем новый файл или перезаписываем существующий)
        FILE_ATTRIBUTE_NORMAL,  // Атрибуты файла (обычный файл)
        NULL                    // Дескриптор файла-шаблона (не используется)
        );

    // Проверка на успешность открытия файла
    if (myFile == INVALID_HANDLE_VALUE)
        return false;

    // Чтение данных
    const DWORD bufferSize = 1024;
    char buffer[bufferSize];
    DWORD bytesRead;

    // Запись данных в буфер
    if(!ReadFile(
        myFile,
        buffer,
        bufferSize,
        &bytesRead,
        NULL
        ))
    {
        return false;
    }

    QByteArray byteArray(buffer, bytesRead);
    QDataStream output(&byteArray, QIODevice::ReadOnly);

    // Очищаем локальную базу перед загрузкой
    database.clear();

    // Зополняем базу данных
    Deposit d;

    while (!output.atEnd()) {
        output >> d;
        database.append(d);
    }

    CloseHandle(myFile);

    return true;

    /*
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
    */
}

// Кол-во элементов
int DepositDatabase::count() {
    return database.count();
}

// Уничтожение всех данных
void DepositDatabase::clear() {
    database.clear();
}
