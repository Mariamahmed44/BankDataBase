#include <QCoreApplication>
#include <Server.h>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Server serverObj;
    serverObj.startServer();

    return a.exec();
}
