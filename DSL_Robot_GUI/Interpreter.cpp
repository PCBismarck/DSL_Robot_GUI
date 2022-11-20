#include "Interpreter.h"

#define VAR 0
#define WORD 1

bool Interpreter::AnalysisScript(QString filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return false;
	
	QTextStream qStream(&file);
	
	//打开文件
	//首先读取变量

	//读取Step
	QString key;
	QStringList buffer;
	//扫描整个脚本文件
	while (!qStream.atEnd())
	{
		//读取一行文件
		//读取行首关键字
		buffer = qStream.readLine().split(' ', Qt::SkipEmptyParts);
		if (buffer.size())
		{
			key = buffer.first().trimmed();
			buffer.removeFirst();
		}
		if (key == "Set")
		{
			QString name = buffer.first().trimmed();
			buffer.removeFirst();
			QString type = buffer.first().trimmed();
			buffer.removeFirst();
			if (type == "Var")
			{
				var.noteTable[name] = QPair<int, int>(VAR, var.SIZE(VAR));
				int value = buffer.first().trimmed().toInt();
				buffer.removeFirst();
				var.val.push_back(value);
			}
			else if (type == "Word")
			{
				var.noteTable[name] = QPair<int, int>(VAR, var.SIZE(WORD));
				QString value = buffer.first().trimmed();
				buffer.removeFirst();
				var.word.push_back(value);
			}
		}
		else if (key == "Entry")
		{
			entry = buffer.first().trimmed();
			buffer.removeFirst();
		}
		else if (key == "Step")
		{
			Step new_step;
			if (buffer.size())
			{
				new_step.name = buffer.first().trimmed();
				buffer.removeFirst();
			}
			//若后续还有说明有多余的字符

			//读取并生成Step中的内容
			GenerateStep(new_step, qStream);
			stepList.push_back(new_step);
		}
		buffer.clear();
		key = "";
	}
	//构造StepPos哈希表
	for (int pos = 0; pos < stepList.size(); ++pos)
		stepPos[stepList[pos].name] = pos;

	return true;
}

QVector<Step>& Interpreter::StepList()
{
	return stepList;
}

int Interpreter::Start()
{
	int cur_step = stepPos[entry];
	for (auto & iter : stepList)
	{
		qDebug() << "Step: " << iter.name;
		iter.Run();
	}
	return 1;
}

bool Interpreter::GenerateStep(Step& step_to_create, QTextStream& qstream)
{
	QStringList buffer; 
	QString key;
	while (!qstream.atEnd())
	{
		buffer = qstream.readLine().split(' ', Qt::SkipEmptyParts);
		if (buffer.size())
		{
			key = buffer.first().trimmed();
			buffer.removeFirst();
		}
		//进入Action处理流程
		if (key == "Speak")
			SpeakProcess(step_to_create, buffer);
		else if (key == "Branch")
			BranchProcess(step_to_create, buffer);
		else if (key == "Listen")
			ListenProcess(step_to_create, buffer);
		else if (key == "Silence")
			SilenceProcess(step_to_create, buffer);
		else if (key == "Modify")
			ModifyProcess(step_to_create, buffer);
		else if (key == "Exit")
			ExitProcess(step_to_create, buffer);
		else if (key == "Default")
			DefaultProcess(step_to_create, buffer);
		else if (key == "End")
			return true;
		else//错误处理
		{
			//break;
		}
		buffer.clear();
		key = "";
	}
	return true;
}

bool Interpreter::SpeakProcess(Step& step_to_modify, QStringList& buffer)
{
	Speak *new_speak = new Speak;
	QString word;
	while (buffer.size())
	{
		word = buffer.first().trimmed();
		buffer.removeFirst();
		new_speak->words.push_back(word);
	}
	step_to_modify.behavior.push_back(new_speak);
	return true;
}

bool Interpreter::BranchProcess(Step& step_to_modify, QStringList& buffer)
{
	Branch *new_branch = new Branch;
	QString token;
	while (buffer.size())
	{
		token = buffer.first().trimmed();
		buffer.removeFirst();
		int delim_pos = token.indexOf(':');
		QString key = token.left(delim_pos);
		QString step_name = token.mid(delim_pos + 1, -1);
		new_branch->jump[key] = step_name;
	}
	step_to_modify.behavior.push_back(new_branch);
	return true;
}

bool Interpreter::ListenProcess(Step& step_to_modify, QStringList& buffer)
{
	Listen *new_listen = new Listen;
	QString token = buffer.first().trimmed();
	buffer.removeFirst();
	int delim_pos = token.indexOf(',');
	QString start = token.left(delim_pos);
	QString end = token.mid(delim_pos + 1, -1);
	new_listen->start_time = start.toInt();
	new_listen->end_time = end.toInt();
	step_to_modify.behavior.push_back(new_listen);
	return true;
}

bool Interpreter::SilenceProcess(Step& step_to_modify, QStringList& buffer)
{
	Silence *new_silence = new Silence;
	QString next_step = buffer.first().trimmed();
	buffer.removeFirst();
	new_silence->jumpTo = next_step;
	step_to_modify.behavior.push_back(new_silence);
	return true;
}

bool Interpreter::ExitProcess(Step& step_to_modify, QStringList& buffer)
{
	Exit *new_exit = new Exit;
	step_to_modify.behavior.push_back(new_exit);
	return true;
}

bool Interpreter::ModifyProcess(Step& step_to_modify, QStringList& buffer)
{
	//等待补全
	Modify* new_modify = new Modify;
	step_to_modify.behavior.push_back(new_modify);
	return true;
}

bool Interpreter::DefaultProcess(Step& step_to_modify, QStringList& buffer)
{
	Default *new_default = new Default;
	QString next_step = buffer.first().trimmed();
	buffer.removeFirst();
	new_default->jumpTo = next_step;
	step_to_modify.behavior.push_back(new_default);
	return true;
}

int Speak::Execute()
{
	//for (auto & iter : words)
	//{
	//	qDebug() << iter << " ";
	//}
	qDebug() << "Speak Execute!" ;
	return -1;
}

int Branch::Execute()
{
	qDebug() << "Branch Execute!";
	return -1;
}

int Listen::Execute()
{
	qDebug() << "Listen Execute!";
	return -1;
}

int Exit::Execute()
{
	qDebug() << "Exit Execute!";
	return -1;
}

int Silence::Execute()
{
	qDebug() << "Silence Execute!";
	return -1;
}

int Default::Execute()
{
	qDebug() << "Default Execute!";
	return -1;
}

int Modify::Execute()
{
	qDebug() << "Modify Execute!";
	return -1;
}

int VarList::SIZE(int type)
{
	if (type == VAR)
		return val.size();
	else if(type == WORD)
		return word.size();
	return -1;
}

int Step::Run()
{
	int cur_action = 0;
	for (auto &iter : behavior)
	{
		iter->Execute();
	}
	return -1;
}
