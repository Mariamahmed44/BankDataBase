#ifndef SERVERHANDLER_H
#define SERVERHANDLER_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDateTime>

class ServerHandler : public QThread
{
    Q_OBJECT
public:
    explicit ServerHandler(int Id,QObject *parent = nullptr);
    void sendMessageToClient(QString message);
    void Operations(QString operation,QString bal,QString num);
    QString accnum(QString str);
    QJsonDocument readJsonFile();
    QJsonDocument readJsonFileDB();
    QString balance(QString str);
    QString transac(QString str);
    bool writeJsonToFileDB(const QJsonObject& jsonObj);
    void displayDB();
    bool deleteUser(const QString &accountNumber);
    bool deleteUserLogin(const QString &accountNumber);
    bool writeJsonToFile(const QJsonObject& jsonObj);
    void createUser(const QString &userDetails);
    void updateUser(const QString &input);
public slots:
    void onReadWrite_slot();
    void onDisconnect_slot();

private:
    int ID;
    QTcpSocket *socket_ptr;
    // QThread interface
protected:
    void run();
};

#endif // SERVERHANDLER_H
