#include <QCoreApplication>
#include <depositDatabase.h>
#include <clocale>
#include <Windows.h>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    setlocale(LC_ALL, "Russian");
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    DepositDatabase dd;
    dd.start();

    QCoreApplication::quit();

    return a.exec();
}
