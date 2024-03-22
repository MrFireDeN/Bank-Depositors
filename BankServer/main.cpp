#include <QCoreApplication>
#include <depositDatabase.h>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DepositDatabase dd;
    dd.start();

    return a.exec();
}
