#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&client,&MyClient::Connection,this,&MainWindow::onConnectionDevice);
    connect(&client,&MyClient::Disconnected,this,&MainWindow::onDisconnectedDevice);
    connect(&client,&MyClient::ErrorOccurred,this,&MainWindow::onErrorOccurredDevice);
    connect(&client,&MyClient::StateChanged,this,&MainWindow::onStateChangedDevice);
    connect(&client,&MyClient::ReadyRead,this,&MainWindow::onReadyReadDevice);
    ui->tabWidget->setTabEnabled(2, false);
    ui->tabWidget->setTabEnabled(3, false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onConnectionDevice()
{
    ui->lwData->addItem("Client has connected to the server....");
}

void MainWindow::onDisconnectedDevice()
{
    ui->lwData->addItem("Client has disconnected from the server....");
}

void MainWindow::onErrorOccurredDevice(QAbstractSocket::SocketError socketError)
{
    QMetaEnum meta = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    ui->lwData->addItem(meta.valueToKey(socketError));
}

void MainWindow::onStateChangedDevice(QAbstractSocket::SocketState socketState)
{
    QMetaEnum meta = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    ui->lwData->addItem(meta.valueToKey(socketState));
}

void MainWindow::onReadyReadDevice(QString data)
{
    QString str = QString("Data Recieved => %1").arg(data);
    ui->lwData->addItem(str);
    ui->lwData_2->addItem(str);
    ui->lwData_3->addItem(str);
}

void MainWindow::on_pbConnect_clicked()
{
    QString ip = ui->leIP->text();
    qint32 port = ui->lePort->text().toInt();
    client.ConnectToDevice(ip,port);
}


void MainWindow::on_pbDisconnect_clicked()
{
    client.Disconnect();
}


void MainWindow::on_pbSend_clicked()
{
    QString data = ui->leData->text();

    client.WriteData(data);
}


void MainWindow::on_pushButton_clicked()
{
     name = ui->lineEdit->text();
     pass = ui->lineEdit_2->text();

    QJsonDocument jsonDoc = client.readJsonFile();
    if (client.authenticate(name,pass,jsonDoc))
    {
        QMessageBox::information(this, "Message", "Right");

        if(client.flag==2)
        {
            ui->tabWidget->setTabEnabled(3, true);
            ui->tabWidget->setTabEnabled(2, false);
        }
        else
        {
            ui->tabWidget->setTabEnabled(2, true);
            ui->tabWidget->setTabEnabled(3, false);
        }
    }
    else
    {
        QMessageBox::warning(this, "Message", "Wrong");
    }
}


void MainWindow::on_pushButton_11_clicked()  //get acc number for user  # for user $ for admin
{
    QString data = "#"+name+pass;

    client.WriteData(data);
    qDebug()<<data;
}


void MainWindow::on_pushButton_10_clicked()   //get balance number for user  % for user ^ for admin
{
    QString data = "%"+name+pass;

    client.WriteData(data);
    qDebug()<<data;
}


void MainWindow::on_pushButton_9_clicked()     //get transac history for user  ! for user @ for admin
{
    QString data = "!"+name+pass;

    client.WriteData(data);
    qDebug()<<data;
}


void MainWindow::on_pushButton_13_clicked()  //make transac  for user *
{
    QString data ="*"+name+pass+ui->lePort_2->text();

    client.WriteData(data);
    qDebug()<<data;
}


void MainWindow::on_pushButton_12_clicked()   //make transfer  for user &   //&22-2222-2223
{
    QString data ="&"+ui->lePort_5->text()+ui->lePort_4->text()+ui->lePort_3->text();

    client.WriteData(data);
    qDebug()<<data;
}


void MainWindow::on_pushButton_2_clicked()   //display database ~
{
    QString data ="~";

    client.WriteData(data);
    qDebug()<<data;
}


void MainWindow::on_pushButton_8_clicked()  //history for admin @
{
    QString data ="@"+ui->lineEdit_35->text();   //@2222

    client.WriteData(data);
    qDebug()<<data;
}


void MainWindow::on_pushButton_5_clicked()  //balance for admin  ^
{
    QString data ="^"+ui->lineEdit_29->text();  // @2222

    client.WriteData(data);
    qDebug()<<data;
}


void MainWindow::on_pushButton_4_clicked()  //accnum for admin $
{
    QString data ="$"+ui->lineEdit_21->text()+ui->lineEdit_22->text();

    client.WriteData(data);
    qDebug()<<data;
}


void MainWindow::on_pushButton_7_clicked()  //for deletion
{
    QString data ="="+ui->lineEdit_11->text();

    client.WriteData(data);
    qDebug()<<data;
}


void MainWindow::on_pushButton_3_clicked()  //create user  //_:name:pass:num:age:full:balance
{
    QString data ="_:"+ui->lineEdit_3->text()+":"+ui->lineEdit_4->text()+":"+ui->lineEdit_5->text()+":"+ui->lineEdit_6->text()+":"+ui->lineEdit_7->text()+":"+ui->lineEdit_8->text();

    client.WriteData(data);
    qDebug()<<data;
}


void MainWindow::on_pushButton_6_clicked()  //update user  //?:name:pass:num:age:full:balance   pass-age-full-balance can be changed
{
    QString data ="?:"+ui->lineEdit_15->text()+":"+ui->lineEdit_16->text()+":"+ui->lineEdit_17->text()+":"+ui->lineEdit_18->text()+":"+ui->lineEdit_19->text()+":"+ui->lineEdit_20->text();

    client.WriteData(data);
    qDebug()<<data;
}

