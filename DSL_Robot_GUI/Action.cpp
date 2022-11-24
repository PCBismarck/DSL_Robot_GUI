#include "Action.h"

int Speak::Execute(QString &OBuffer, VarList& var, QTcpSocket* client)
{
	//QThread::sleep(0.5);
	qDebug() << "Speak Execute!";
	QString token;
	for (auto& word : words)
	{
		if (word.first(1) == "$")//是变量
		{
			//qDebug() << var.noteTable;
			QPair<int, int> varInfo = var.noteTable[word.mid(1, -1)];
			switch (varInfo.first)
			{
			case VAR:
				token.append(QString::number(var.val[varInfo.second]));
				break;
			case WORD:
				token.append(var.word[varInfo.second]);
				break;
			default:
				break;
			}
		}
		else
			token.append(word);
	}
	client->write(token.toLocal8Bit());
	client->waitForBytesWritten();
	//QThread::sleep(0.5);
	qDebug()  << "Send:" << token;
	return NORM;
}

int Branch::Execute(QString &OBuffer, VarList& var, QTcpSocket* client)
{
	qDebug() << "Branch Execute!";
	if (OBuffer == "\n\t\n@Silence")
		return NORM;
	QStringList keys = jump.keys();
	QStringList varKeys = var.noteTable.keys();
	qDebug() << keys << varKeys;
	if (!varKeys.contains("Heard"))
		return EROR;
	QPair<int, int> heard_info = var.noteTable["Heard"];
	if (heard_info.first == VAR)
		return EROR;
	QString Heard = var.word[heard_info.second];
	for (auto& key : keys)
	{
		if (Heard.contains(key))
		{
			OBuffer = jump[key];
			qDebug() << "Jump to " << OBuffer;
			return JUMP;
		}
	}
	return NORM;
}

int Listen::Execute(QString &OBuffer, VarList& var, QTcpSocket* client)
{
	QString heard;
	QStringList keys = var.noteTable.keys();
	//监听是否有输入
	client->waitForReadyRead(waitTime*1000);
	heard = QString::fromLocal8Bit(client->readAll());
	qDebug() << "Listen: " << heard;
	OBuffer.clear();
	if (heard.isEmpty())
	{
		OBuffer = "\n\t\n@Silence";
		return HEAR;
	}
	//Heard已经存在
	if (keys.contains("Heard"))
	{
		QPair<int, int> varInfo = var.noteTable["Heard"];
		int type = varInfo.first;
		int pos = varInfo.second;
		if (type == heardType)//与原来的类型相同
		{
			//直接修改
			if (type == VAR)
			{
				if (heard.isEmpty())
					var.val[pos] = 0;
				else
					var.val[pos] = heard.toInt();
			}
			else if (type == WORD)
			{
				if (heard.isEmpty())
					var.word[pos] = "";
				else
					var.word[pos] = heard;
			}
			return HEAR;
		}
		else//与原来的类型不同，删除原有值和索引
		{
			if (type == VAR)
				var.val.removeLast();
			else if (type == WORD)
				var.word.removeLast();
			var.noteTable.remove("Heard");
		}
	}
	//heard 没有加入变量表
	if (heardType == VAR)
	{
		var.noteTable["Heard"] = QPair<int, int>(VAR, var.word.size());
		if (heard.isEmpty())
			var.val.push_back(0);
		else
			var.val.push_back(heard.toInt());
	}
	else
	{
		var.noteTable["Heard"] = QPair<int, int>(WORD, var.word.size());
		if (heard.isEmpty())
			var.word.push_back("");
		else
			var.word.push_back(heard);
	}
	QString socketIpAddress = client->peerAddress().toString();
	int port = client->peerPort();
	qDebug() << "Listen Execute!" << heard;
	qDebug() << "Message: " + heard + " (" + socketIpAddress + ":" + QString::number(port) + ")";
	return HEAR;
}

int Exit::Execute(QString &OBuffer, VarList& var, QTcpSocket* client)
{
	OBuffer.clear();
	qDebug() << "Exit Execute!";
	return EXIT;
}

int Silence::Execute(QString &OBuffer, VarList& var, QTcpSocket* client)
{
	if (OBuffer == "\0\n@Silence")
		return NORM;
	OBuffer.clear();
	qDebug() << "Silence Execute!";
	OBuffer = jumpTo;
	return JUMP;
}

int Default::Execute(QString &OBuffer, VarList& var, QTcpSocket* client)
{
	OBuffer.clear();
	OBuffer = jumpTo;
	qDebug() << "Default Execute!";
	return JUMP;
}

int Modify::Execute(QString &OBuffer, VarList& var, QTcpSocket* client)
{
	qDebug() << var.noteTable << var.val << var.word << opQue << varQue << toModify;
	if (toModify.first(1) != "$")
		return EROR;
	QPair<int, int> varInfo = var.noteTable[toModify.mid(1, -1)];
	int type = varInfo.first;
	int pos = varInfo.second;
	int cur_var_pos = 0, cur_op_pos = 0;
	if (type == VAR)
	{
		int cur_val;
		if (varQue.size() == cur_var_pos)
			return EROR;
		QString key = varQue[cur_var_pos++];
		if (key.first(1) == "$")
			key = key.mid(1, -1);
		QPair<int, int> v_info = var.noteTable[key];
		if (v_info.first != VAR)
			return EROR;
		cur_val = var.val[v_info.second];
		while (opQue.size() != cur_op_pos)
		{
			int temp;
			if (varQue.size() == cur_var_pos)
				return EROR;
			key = varQue[cur_var_pos++];
			if (key.first(1) == "$")
			{
				QPair<int, int> temp_info = var.noteTable[key.mid(1, -1)];
				if (v_info.first != VAR)
					return EROR;
				temp = var.val[temp_info.second];
			}
			else
				temp = key.toInt();
			QString op = opQue[cur_op_pos++];
			if (op == "+")
				cur_val += temp;
			else if (op == "-")
				cur_val -= temp;
			else if (op == "*")
				cur_val *= temp;
			else if (op == "/" && temp != 0)
				cur_val /= temp;
			else
				return EROR;
		}
		var.val[pos] = cur_val;
	}
	else if (type == WORD)
	{
		QString cur_str;
		if (varQue.size() == cur_var_pos)
			return EROR;
		QString key = varQue[cur_var_pos++];
		if (key.first(1) == "$")
			key = key.mid(1, -1);
		QPair<int, int> v_info = var.noteTable[key];
		if (v_info.first != WORD)
			return EROR;
		cur_str = var.word[v_info.second];
		while (opQue.size() != cur_op_pos)
		{
			QString temp;
			if (varQue.size() == cur_var_pos)
				return EROR;
			key = varQue[cur_var_pos++];
			if (key.first(1) == "$")
			{
				QPair<int, int> temp_info = var.noteTable[key.mid(1, -1)];
				if (v_info.first != WORD)
					return EROR;
				temp = var.word[temp_info.second];
			}
			else
				temp = key;
			QString op = opQue[cur_op_pos++];
			if (op == "+")
				cur_str += temp;
			else
				return EROR;
		}
		var.word[pos] = cur_str;
	}
	qDebug() << "Modify Execute!";
	return NORM;
}

int VarList::SIZE(int type)
{
	if (type == VAR)
		return val.size();
	else if (type == WORD)
		return word.size();
	return EROR;
}

int Step::Run(QString& SOBuffer, VarList& var, QTcpSocket* client)
{
	for (auto& iter : behavior)
	{
		int state = iter->Execute(SOBuffer, var, client);
		switch (state)
		{
		case EXIT: 
		case EROR:
			return state;
		case JUMP:
			return JUMP;
		case HEAR:
			break;
		default:
			break;
		}
		qDebug() << name << ": Next action!";
		if (client->state() == QTcpSocket::UnconnectedState)
			return EROR;
	}
	return NORM;
}