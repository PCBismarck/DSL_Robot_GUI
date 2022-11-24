#include "Interpreter.h"
#include "Action.h"


bool Interpreter::AnalysisScript(QString filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return false;
	
	QTextStream qStream(&file);
	
	//打开文件
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
			QString type = buffer.first().trimmed();
			buffer.removeFirst();
			QString name = buffer.first().trimmed();
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
				var.noteTable[name] = QPair<int, int>(WORD, var.SIZE(WORD));
				QString value = buffer.first().trimmed().remove("\"");
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
	//qDebug() << stepPos << var.noteTable;
	return true;
}

QVector<Step>& Interpreter::StepList()
{
	return stepList;
}

bool Interpreter::GenerateStep(Step& step_to_create, QTextStream& qstream)
{
	QStringList buffer; 
	QString key;
	while (!qstream.atEnd())
	{
		QRegularExpression regex("[\t ]");
		//读取首关键字
		qstream >> key;
		//进入Action处理流程
		if (key == "Speak")
		{
			buffer = qstream.readLine().split('+', Qt::SkipEmptyParts);
			SpeakProcess(step_to_create, buffer);
		}
		else if (key == "Branch")
		{
			buffer = qstream.readLine().split(regex, Qt::SkipEmptyParts);
			BranchProcess(step_to_create, buffer);
		}
		else if (key == "Listen")
		{
			buffer = qstream.readLine().split(regex, Qt::SkipEmptyParts);
			ListenProcess(step_to_create, buffer);
		}
		else if (key == "Silence")
		{
			buffer = qstream.readLine().split(regex, Qt::SkipEmptyParts);
			SilenceProcess(step_to_create, buffer);
		}
		else if (key == "Modify")
		{
			QRegularExpression exp;
			exp.setPattern("[/+/-/*//]");
			QString express = qstream.readLine();
			qDebug() << express;
			int cur_pos = 0 , pre_pos = 0;
			cur_pos = express.indexOf(":");
			buffer << express.left(cur_pos).trimmed();
			pre_pos = cur_pos + 1;
			cur_pos = express.indexOf(exp, pre_pos);
			while (cur_pos >= 0)
			{
				buffer << express.mid(pre_pos, cur_pos - pre_pos).trimmed() << express[cur_pos];
				pre_pos = cur_pos + 1;
				cur_pos = express.indexOf(exp, pre_pos);
			}
			buffer << express.last(express.size() - pre_pos).trimmed();
			qDebug() << buffer;
			ModifyProcess(step_to_create, buffer);
		}
		else if (key == "Exit")
		{
			buffer = qstream.readLine().split(regex, Qt::SkipEmptyParts);
			ExitProcess(step_to_create, buffer);
		}
		else if (key == "Default")
		{
			buffer = qstream.readLine().split(regex, Qt::SkipEmptyParts);
			DefaultProcess(step_to_create, buffer);
		}
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
		word = buffer.first().trimmed().replace("\"", "");
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
		QString key = token.left(delim_pos).trimmed().replace("\"", "");
		QString step_name = token.mid(delim_pos + 1, -1).trimmed().replace("\"", "");
		new_branch->jump[key] = step_name;
	}
	qDebug() << new_branch->jump;
	step_to_modify.behavior.push_back(new_branch);
	return true;
}

bool Interpreter::ListenProcess(Step& step_to_modify, QStringList& buffer)
{
	Listen *new_listen = new Listen;
	QString type = buffer.first().trimmed();
	buffer.removeFirst();
	if (type == "Var")
		new_listen->heardType = VAR;
	else if (type == "Word")
		new_listen->heardType = WORD;
	else
		return false;
	QString token = buffer.first().trimmed();
	buffer.removeFirst();
	new_listen->waitTime = token.toInt();
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
	Modify* new_modify = new Modify;
	new_modify->toModify = buffer.first().trimmed();
	buffer.removeFirst();
	int is_val = 1;
	for (auto & token : buffer)
	{
		if (is_val)
			new_modify->varQue.push_back(token);
		else
			new_modify->opQue.push_back(token);
		is_val = 1 - is_val;
	}
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




