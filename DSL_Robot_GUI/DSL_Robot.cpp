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
	connect(client, &QTcpSocket::disconnected, this, &DSL_Robot::HandleDisconnect);
	qDebug() << client;
	QString msg = tr("大雄你又有什么问题?");
	client->write(msg.toLocal8Bit());
	client->waitForBytesWritten();
	QThread::sleep(1);

	//初始化变量表
	//MyVal = SynTree->var;
	Interpreter* tempTree = SynTree;
	copy_varList(&tempTree->var);
	//获取初始步骤
	Step* cur_step;
	QString next_step_name;
	QString OBuffer;
	int cur_step_seq = tempTree->stepPos[tempTree->entry];
	cur_step = &(tempTree->stepList[cur_step_seq]);
	while (true)
	{
		qDebug() << "Cuurent Step" << cur_step->name;
		int state = cur_step->Run(OBuffer, MyVal, client);
		switch (state)
		{
		case EXIT:
			msg = "会话已结束，您可以断开连接";
			send_msg(client, msg);
			if (!client->waitForDisconnected())
				client->abort();
			return;
		case EROR:
			qDebug() << "EROR";
			msg = "抱歉出现了未知的错误，您可以联系管理员";
			send_msg(client, msg);
			if (!client->waitForDisconnected())
				client->abort();
			return;
		case JUMP:
			next_step_name = OBuffer;
			qDebug() << "STEP JUMP TO" << OBuffer;
			if (tempTree->stepPos.keys().contains(next_step_name))//如果下一步索引存在
			{
				cur_step_seq = tempTree->stepPos[next_step_name];
				cur_step = &tempTree->stepList[cur_step_seq];
				break;
			}
			qDebug() << "JUMP ERROR";
			msg = "抱歉出现了未知的错误，您可以联系管理员";
			send_msg(client, msg);
			if (!client->waitForDisconnected())
				client->abort();
			return;
		case HEAR:
		case NORM:
			break;
		default:
			break;
		}
		QThread::sleep(1);
	}
		msg = "会话已结束，您可以断开连接";
		send_msg(client, msg);
		if (!client->waitForDisconnected())
			client->abort();
	return;
}

void DSL_Robot::HandleDisconnect()
{
	return;
}

void DSL_Robot::copy_varList(VarList* to_copy)
{
	for (auto & iter : to_copy->val)
		MyVal.val.push_back(iter);
	for (auto& iter : to_copy->word)
		MyVal.word.push_back(iter);
	QStringList keys = to_copy->noteTable.keys();
	for (auto& iter : keys)
		MyVal.noteTable[iter] = to_copy->noteTable[iter];
}

