#pragma once

#include <QtWidgets/QMainWindow>
#include <QDateTime>
#include "ui_DSL_Robot_GUI.h"
#include "Interpreter.h"
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtConcurrent>
#include "DSL_Robot.h"

class TcpServerThr : public QTcpServer
{
	Q_OBJECT
public:
	explicit TcpServerThr(QObject* parent = nullptr);

private:
	void incomingConnection(qintptr socketDescriptor);

signals:
	void signalNewConnection(qintptr socketDescriptor);
    //void signalNewConnection(DSL_Robot*);

};


class DSL_Robot_GUI : public QMainWindow
{
    Q_OBJECT

public:
    DSL_Robot_GUI(QWidget *parent = nullptr);
    ~DSL_Robot_GUI();

private slots:
    //void on_sendButton_clicked();
    //void handleNewConnection();
    //void recv_msg();
    void start_new_thread(qintptr socketDescriptor);

private:
    Ui::DSL_Robot_GUIClass ui;
    TcpServerThr* server;
    QVector<QTcpSocket*>* allClients;
    Interpreter SynTree;
    QStringList IBuffer;
};

