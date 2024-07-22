#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <MyClient.h>
#include <QMetaEnum>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    QString name ;
    QString pass ;
    ~MainWindow();


public slots:
    void onConnectionDevice();
    void onDisconnectedDevice();
    void onErrorOccurredDevice(QAbstractSocket::SocketError socketError);
    void onStateChangedDevice(QAbstractSocket::SocketState socketState);
    void onReadyReadDevice(QString data);
private slots:
    void on_pbConnect_clicked();

    void on_pbDisconnect_clicked();

    void on_pbSend_clicked();

    void on_pushButton_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_13_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_6_clicked();

private:
    Ui::MainWindow *ui;
    MyClient client;
};
#endif // MAINWINDOW_H
