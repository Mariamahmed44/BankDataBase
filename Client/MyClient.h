#ifndef MYCLIENT_H
#define MYCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
class MyClient : public QObject
{
    Q_OBJECT
public:
    explicit MyClient(QObject *parent = nullptr);
    void ConnectToDevice(QString ip,qint32 port);
    void Disconnect();
    void WriteData(QString data);
    QJsonDocument readJsonFile();
    bool authenticate(const QString &name, const QString &password, const QJsonDocument &jsonDoc);
    int flag=0; //1 for admin 2 for user


signals:
    void Connection();
    void Disconnected();
    void ErrorOccurred(QAbstractSocket::SocketError socketError);
    void StateChanged(QAbstractSocket::SocketState socketState);
    void ReadyRead(QString data);
private slots:
    void onConnection();
    void onDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);
    void onStateChanged(QAbstractSocket::SocketState socketState);
    void onReadyRead();
private:
    QString ip;
    qint32 port;
    QTcpSocket socket;
};

#endif // MYCLIENT_H
