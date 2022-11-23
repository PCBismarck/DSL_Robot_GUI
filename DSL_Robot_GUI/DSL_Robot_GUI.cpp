#include "DSL_Robot_GUI.h"

DSL_Robot_GUI::DSL_Robot_GUI(QWidget *parent)
    : QMainWindow(parent)
{
	SynTree.AnalysisScript("E:/desktop/DSL-Robot/testGrammar.dsl");
    ui.setupUi(this);
	allClients = new QVector<QTcpSocket*>;
	server = new TcpServerThr();
	server->setMaxPendingConnections(10);
	connect(server, &TcpServerThr::signalNewConnection, this, &DSL_Robot_GUI::start_new_thread,
		Qt::QueuedConnection);
	QHostAddress hostAdd;
	hostAdd.setAddress("127.0.0.1");
	int port = 7777;
	if (server->listen(hostAdd, port))
		qDebug() << "Server start up" << hostAdd;

}

DSL_Robot_GUI::~DSL_Robot_GUI()
{}

void DSL_Robot_GUI::start_new_thread(qintptr socketDescriptor)
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