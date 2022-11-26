#include "DSL_Robot_Server.h"
#include <iostream>

DSL_Robot_Server::DSL_Robot_Server(QString ScriptPath, QWidget *parent)
    : QMainWindow(parent)
{
	if (!SynTree.AnalysisScript(ScriptPath))
	{
		qDebug() << "Error：找不到脚本文件";
		exit(-1);
	}
    ui.setupUi(this);
	server = new TcpServerThr();
	server->setMaxPendingConnections(1000);
	connect(server, &TcpServerThr::signalNewConnection, this, &DSL_Robot_Server::start_new_thread,
		Qt::QueuedConnection);
	QHostAddress hostAdd;
	hostAdd.setAddress("127.0.0.1");
	int port = 7777;
	if (server->listen(hostAdd, port))
		std::cout << "Server start up" << hostAdd.toString().toStdString();
}

DSL_Robot_Server::~DSL_Robot_Server()
{}

void DSL_Robot_Server::start_new_thread(qintptr socketDescriptor)
{
	qDebug() << QThread::currentThreadId();
	qDebug() << "start thread" << socketDescriptor;
	DSL_Robot* robot_1 = new DSL_Robot();
	QtConcurrent::run(&DSL_Robot::StartWorking, robot_1, socketDescriptor, &SynTree);
}

TcpServerThr::TcpServerThr(QObject* parent) : QTcpServer(parent)
{

}

void TcpServerThr::incomingConnection(qintptr socketDescriptor)
{
	emit signalNewConnection(socketDescriptor);
	qDebug() << "TcpServerThr::incomingConnection() " << socketDescriptor;
}