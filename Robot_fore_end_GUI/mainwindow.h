#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QTcpSocket>
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>
#include <QListWidgetItem>
#include "ChatWidget.h"

#define HOSTADD "127.0.0.1"
#define HOSTPORT 7777

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void dealMessage(ChatWidget *messageW, QListWidgetItem *item, QString text, QString time, ChatWidget::User_Type type);
    void dealMessageTime(QString curMsgTime);

private slots:
    void on_connectButton_clicked();

    void on_sendButton_clicked();

    void recv_msg();

    void recv_connected_signal();

    void recv_disconnected_signal();

    void on_disconnectButton_clicked();

    void on_cleanButton_clicked();

signals:
//    void recv_connect();

protected:
    int whenTimeShow;

private:
    Ui::MainWindow *ui;
    QTcpSocket* client;
};
#endif // MAINWINDOW_H
