// DSL_Robot.cpp: 定义应用程序的入口点。
//

#include "DSL_Robot.h"

void send_msg(QTcpSocket* client, QString msg)
{
	client->write(msg.toLocal8Bit());
	client->waitForBytesWritten();
}

void DSL_Robot::StartWorking(qintptr socketDescriptor, Interpreter* SynTree)
{
	qDebug() << QThread::currentThreadId();
	qDebug() << "Start Working" << socketDescriptor;
	QTcpSocket* client = new QTcpSocket;
	client->setSocketDescriptor(socketDescriptor);
	qDebug() << client;
	QString msg = "连接成功";
	client->write(msg.toLocal8Bit());
	client->waitForBytesWritten();
	QThread::sleep(1);

	//初始化变量表
	MyVal = SynTree->var;
	//获取初始步骤
	Step* cur_step;
	QString next_step_name;
	QString OBuffer;
	int cur_step_seq = SynTree->stepPos[SynTree->entry];
	cur_step = &(SynTree->stepList[cur_step_seq]);
	while (true)
	{
		qDebug() << cur_step->name;
		int state = SynTree->stepList[cur_step_seq].Run(OBuffer, MyVal, client);
		switch (state)
		{
		case EXIT:
			msg = "会话已结束，您可以断开连接";
			send_msg(client, msg);
			client->waitForDisconnected();
			return;
		case EROR:
			qDebug() << "EROR";
			msg = "抱歉出现了未知的错误，您可以联系管理员";
			send_msg(client, msg);
			client->waitForDisconnected();
			return;
		case JUMP:
			next_step_name = OBuffer;
			qDebug() << "STEP JUMP TO" << OBuffer;
			if (SynTree->stepPos.keys().contains(next_step_name))//如果下一步索引存在
			{
				cur_step_seq = SynTree->stepPos[next_step_name];
				cur_step = &SynTree->stepList[cur_step_seq];
				break;
			}
			qDebug() << "JUMP ERROR";
			msg = "抱歉出现了未知的错误，您可以联系管理员";
			send_msg(client, msg);
			client->waitForDisconnected();
			return;
		default:
			break;
		}
	}
		msg = "会话已结束，您可以断开连接";
		send_msg(client, msg);
		client->waitForDisconnected();
	return;
}

