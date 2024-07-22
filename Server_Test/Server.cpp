#include "Server.h"

Server::Server(QObject *parent)
    : QTcpServer{parent},qin{stdin},qout{stdout}
{}



void Server::startServer()
{
    qout<<"Enter server port Id to listen: ";
    qout.flush();
    port = qin.readLine().toInt();

    if (this->listen(QHostAddress::Any, port))
    {
        qDebug() << "Server is listening to the port: " << port;
    }
    else
    {
        qDebug() << "Server cannot listen to port: " << port;
    }


}

void Server::incomingConnection(qintptr handle)
{
    qDebug()<<"Client "<<handle <<"is Conntected to the server\n";

    ServerHandler *server_handler_ptr = new ServerHandler(handle,this);

    connect(server_handler_ptr,&QThread::finished,server_handler_ptr,&QThread::deleteLater);

    server_handler_ptr->start();
}
