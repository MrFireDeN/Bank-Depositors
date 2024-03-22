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
        GENERIC_WRITE,          // Желаемый доступ к файлу (здесь только запись)
        0,                      // Режим разделения (нельзя открывать другим процессам)
        NULL,                   // Атрибуты безопасности (не используется)
        CREATE_ALWAYS,          // Режим открытия (открываем существующий файл)
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
    DWORD bytesRead;
    int counts = database.count();

    WriteFile(myFile, &counts, sizeof(counts), &bytesRead, NULL);


    for (i = database.end()-1; i != database.begin()-1; --i) {
        d = Deposit::toStruct(*i);

        // id
        WriteFile(myFile, &d.id, sizeof(d.id), &bytesRead, NULL);
        // Номер счета
        WriteFile(myFile, &d.accountNumber, sizeof(d.accountNumber), &bytesRead, NULL);
        // Тип вклада
        WriteFile(myFile, &d.type, sizeof(d.type), &bytesRead, NULL);
        // ФИО
        WriteFile(myFile, &d.FIO, sizeof(d.FIO), &bytesRead, NULL);
        // Дата рождения
        WriteFile(myFile, &d.birthDate, sizeof(d.birthDate), &bytesRead, NULL);
        // Сумма вклада
        WriteFile(myFile, &d.amount, sizeof(d.amount), &bytesRead, NULL);
        // Процент вклада
        WriteFile(myFile, &d.interest, sizeof(d.interest), &bytesRead, NULL);
        // Переодичность начисления
        WriteFile(myFile, &d.accrualFrequency, sizeof(d.accrualFrequency), &bytesRead, NULL);
        // Последняя транзакция
        WriteFile(myFile, &d.lastTransaction, sizeof(d.lastTransaction), &bytesRead, NULL);
        // Наличие пластиковой карты
        WriteFile(myFile, &d.plasticCardAvailability, sizeof(d.plasticCardAvailability), &bytesRead, NULL);
    }

    CloseHandle(myFile);

    return true;
}

// Загрузить файл базы данных
bool DepositDatabase::load() {
    // Создание файла
    HANDLE myFile = CreateFile(
        FILENAME,               // Имя файла
        GENERIC_READ,           // Желаемый доступ к файлу (здесь только чтение)
        0,                      // Режим разделения (нельзя открывать другим процессам)
        NULL,                   // Атрибуты безопасности (не используется)
        OPEN_ALWAYS,            // Режим создания (создаем новый файл или перезаписываем существующий)
        FILE_ATTRIBUTE_NORMAL,  // Атрибуты файла (обычный файл)
        NULL                    // Дескриптор файла-шаблона (не используется)
        );

    // Проверка на успешность открытия файла
    if (myFile == INVALID_HANDLE_VALUE)
        return false;

    // Очищаем локальную базу перед загрузкой
    database.clear();

    //QVector<Deposit>::iterator i;
    Deposit::D d;
    DWORD bytesRead;
    int counts;

    if (!ReadFile(myFile, (void*)&counts, sizeof(counts), &bytesRead, NULL)) {
        CloseHandle(myFile);
        return false;
    }

    for (int i = 0; i < counts; ++i)
    {
        // id
        ReadFile(myFile, (void*)&d.id, sizeof(d.id), &bytesRead, NULL);
        // Номер счета
        ReadFile(myFile, &d.accountNumber, sizeof(d.accountNumber), &bytesRead, NULL);
        // Тип вклада
        ReadFile(myFile, &d.type, sizeof(d.type), &bytesRead, NULL);
        // ФИО
        ReadFile(myFile, &d.FIO, sizeof(d.FIO), &bytesRead, NULL);
        // Дата рождения
        ReadFile(myFile, &d.birthDate, sizeof(d.birthDate), &bytesRead, NULL);
        // Сумма вклада
        ReadFile(myFile, &d.amount, sizeof(d.amount), &bytesRead, NULL);
        // Процент вклада
        ReadFile(myFile, &d.interest, sizeof(d.interest), &bytesRead, NULL);
        // Переодичность начисления
        ReadFile(myFile, &d.accrualFrequency, sizeof(d.accrualFrequency), &bytesRead, NULL);
        // Последняя транзакция
        ReadFile(myFile, &d.lastTransaction, sizeof(d.lastTransaction), &bytesRead, NULL);
        // Наличие пластиковой карты
        ReadFile(myFile, &d.plasticCardAvailability, sizeof(d.plasticCardAvailability), &bytesRead, NULL);

        database.append(Deposit::fromStruct(d));
    }

    CloseHandle(myFile);

    return true;
}

// Кол-во элементов
int DepositDatabase::count() {
    return database.count();
}

// Уничтожение всех данных
void DepositDatabase::clear() {
    database.clear();
}
