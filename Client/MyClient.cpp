#include "MyClient.h"

MyClient::MyClient(QObject *parent)
    : QObject{parent}
{
    connect(&socket,&QTcpSocket::connected,this,&MyClient::onConnection);
    connect(&socket,&QTcpSocket::disconnected,this,&MyClient::onDisconnected);
    connect(&socket,&QTcpSocket::errorOccurred,this,&MyClient::onErrorOccurred);
    connect(&socket,&QTcpSocket::stateChanged,this,&MyClient::onStateChanged);
    connect(&socket,&QTcpSocket::readyRead,this,&MyClient::onReadyRead);

}

void MyClient::ConnectToDevice(QString ip, qint32 port)
{
    if(socket.isOpen())
    {
        if((this->ip == ip)&&(this->port == port))
        {
            return;
        }
        else
        {
            socket.close();
            this->port = port;
            this->ip = ip;
            socket.connectToHost(this->ip,this->port);
        }
    }
    else
    {
        this->port = port;
        this->ip = ip;
        socket.connectToHost(this->ip,this->port);
    }
}

void MyClient::Disconnect()
{
    if(socket.isOpen())
    {
        socket.close();
    }
}

void MyClient::WriteData(QString data)
{
    if(socket.isOpen())
    {
        socket.write(data.toUtf8());
    }
}

void MyClient::onConnection()
{
    emit Connection();
}

void MyClient::onDisconnected()
{
    emit Disconnected();
}

void MyClient::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
    emit ErrorOccurred(socketError);
}

void MyClient::onStateChanged(QAbstractSocket::SocketState socketState)
{
    if(socketState == QAbstractSocket::UnconnectedState)
    {
        socket.close();
    }
    emit StateChanged(socketState);
}

void MyClient::onReadyRead()
{
    QByteArray byteArray = socket.readAll();
    QString data = QString(byteArray);
    emit ReadyRead(data);
}

QJsonDocument MyClient::readJsonFile()
{  QString path="E:/Client/Client/login.json";
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open JSON file:" << file.errorString();
        return QJsonDocument();
    }

    QByteArray jsonData = file.readAll();
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "Failed to parse JSON:" << parseError.errorString();
        return QJsonDocument();
    }

    return jsonDoc;
}

bool MyClient::authenticate(const QString &name, const QString &password, const QJsonDocument &jsonDoc)
{
    QJsonObject jsonObj = jsonDoc.object();

    // Check admins
    if (jsonObj.contains("admins")) {
        QJsonArray adminsArray = jsonObj["admins"].toArray();
        for (const auto &adminObj : adminsArray) {
            QJsonObject admin = adminObj.toObject();
            if (admin.contains(name)) {
                QJsonObject userData = admin[name].toObject();
                if (userData["password"].toString() == password) {
                    qDebug() << "Admin authentication successful.";
                    flag=1;
                    return true;
                } else {
                    qDebug() << "Incorrect password for admin.";
                    return false;
                }
            }
        }
    }

    // Check users
    if (jsonObj.contains("users")) {
        QJsonArray usersArray = jsonObj["users"].toArray();
        for (const auto &userObj : usersArray) {
            QJsonObject user = userObj.toObject();
            if (user.contains(name)) {
                QJsonObject userData = user[name].toObject();
                if (userData["password"].toString() == password) {
                    qDebug() << "User authentication successful.";
                    flag=2;
                    return true;
                } else {
                    qDebug() << "Incorrect password for user.";
                    return false;
                }
            }
        }
    }

    // If name is not found in either admins or users
    qDebug() << "User not found.";
    return false;
}
