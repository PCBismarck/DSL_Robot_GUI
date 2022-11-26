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

//服务端的主要入口类，在其他线程中创建Robot回复客户
class DSL_Robot_Server : public QMainWindow
{
    Q_OBJECT

public:
    //在构造该类时需要给出一个脚本文件地址，在创建该类时同时创建对应脚本文件的Interpreter类
    DSL_Robot_Server(QString ScriptPath, QWidget *parent = nullptr);
    ~DSL_Robot_Server();
private slots:
    //void on_sendButton_clicked();
    //void handleNewConnection();
    //void recv_msg();
    //接收到连接请求后开启一个新线程，并设置Robot
    void start_new_thread(qintptr socketDescriptor);

private:
    Ui::DSL_Robot_GUIClass ui;
    //Tcp Sever类
    TcpServerThr* server;
    //Interpreter类，解析脚本文件，生成状态机
    Interpreter SynTree;
};

