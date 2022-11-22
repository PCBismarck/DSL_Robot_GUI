#include "Action.h"

int Speak::Execute()
{
	OBuffer.clear();
	QString token;
	for (auto& word : words)
	{
		if (word.first(1) == "$")//是变量
		{
			QPair<int, int> varInfo = var->noteTable[word.mid(1, -1)];
			switch (varInfo.first)
			{
			case VAR:
				token.append(QString::number(var->val[varInfo.second]));
				break;
			case WORD:
				token.append(var->word[varInfo.second]);
				break;
			default:
				break;
			}
		}
		else
			token.append(word);
	}
	OBuffer << token;
	qDebug() << "Speak Execute!";
	return NORM;
}

int Branch::Execute()
{
	OBuffer.clear();
	QStringList keys = jump.keys();
	if (IBuffer.isEmpty())
		return EROR;
	for (auto& token : IBuffer)
	{
		for (auto & key : keys)
		{
			if (token.contains(key))
			{
				OBuffer << jump[key];
				qDebug() << "Branch Execute!" << "Jump to " << OBuffer;
				return JUMP;
			}
		}
	}
	return EROR;
}

int Listen::Execute()
{
	OBuffer.clear();
	QString heard;
	
	for (auto& sentence : IBuffer)
		heard.append(sentence);
	QPair<int, int> varInfo = var->noteTable["Heard"];
	int type = varInfo.first;
	int pos = varInfo.second;

	//heard没有根据类型进行细分

	if (type == NULL && pos == NULL)//heard没有加入变量表
	{
		var->noteTable["Heard"] = QPair<int, int>(WORD, var->word.size());
		var->word.push_back(heard);
	}
	else
	{
		var->word[pos] = heard;
	}
	qDebug() << "Listen Execute!";
	return HEAR;
}

int Exit::Execute()
{
	OBuffer.clear();
	qDebug() << "Exit Execute!";
	return EXIT;
}

int Silence::Execute()
{
	OBuffer.clear();
	qDebug() << "Silence Execute!";
	OBuffer << jumpTo;
	return JUMP;
}

int Default::Execute()
{
	OBuffer.clear();
	OBuffer << jumpTo;
	qDebug() << "Default Execute!";
	return JUMP;
}

int Modify::Execute()
{
	QPair<int, int> varInfo = var->noteTable[toModify.mid(1, -1)];
	int type = varInfo.first;
	int pos = varInfo.second;
	if (type == VAR)
	{
		int cur_val;
		QPair<int, int> v_info = var->noteTable[varQue.dequeue()];
		if (v_info.first != VAR)
			return EROR;
		cur_val = var->val[v_info.second];
		while (!opQue.isEmpty())
		{
			int temp;
			QString key = varQue.dequeue();
			if (key.first(1) == "$")
			{
				QPair<int, int> temp_info = var->noteTable[varQue.dequeue()];
				if (v_info.first != VAR)
					return EROR;
				temp = var->val[temp_info.second];
			}
			else
				temp = key.toInt();
			QString op = opQue.dequeue();
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
		var->val[pos] = cur_val;
	}
	else if (type == WORD)
	{
		QString cur_str;
		QPair<int, int> v_info = var->noteTable[varQue.dequeue()];
		if (v_info.first != WORD)
			return EROR;
		cur_str = var->word[v_info.second];
		while (!opQue.isEmpty())
		{
			QString temp;
			QString key = varQue.dequeue();
			if (key.first(1) == "$")
			{
				QPair<int, int> temp_info = var->noteTable[varQue.dequeue()];
				if (v_info.first != WORD)
					return EROR;
				temp = var->word[temp_info.second];
			}
			else
				temp = key;
			QString op = opQue.dequeue();
			if (op == "+")
				cur_str += temp;
			else
				return EROR;
		}
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

int Step::Run()
{
	for (auto& iter : behavior)
	{
		iter->IBuffer = SIBuffer;
		iter->var = var;
		int state = iter->Execute();
		switch (state)
		{
		case EXIT: 
		case EROR:
			return state;
		case JUMP:
			SOBuffer << iter->OBuffer.first();
			return JUMP;
		case HEAR:
			iter->IBuffer.clear();
			break;
		default:
			break;
		}
	}
	return NORM;
}