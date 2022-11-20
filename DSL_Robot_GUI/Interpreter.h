#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <iostream>
#include <QVector>
#include <QString>
#include <utility>
#include <QHash>
#include <fstream>
#include <sstream>
#include <QDebug>
#include <QFile>

#define BUF_SIZE 1024


class Action
{
public:
	virtual int Execute() = 0;
};

class Speak :public Action
{
public:
	int Execute();
public:
	QVector<QString> words;
};

class Branch : public Action
{
public:
	int Execute();
public:
	QHash<QString, QString> jump;
};

class Listen : public Action
{
	//设置timer
public:
	int Execute();

public:
	int start_time;
	int end_time;
	QString Heard;
};

class Exit : public Action
{
public:
	int Execute();
};

class Silence : public Action
{
public:
	int Execute();
public:
	QString jumpTo;
};

class Default : public Action
{
public:
	int Execute();
public:
	QString jumpTo;
};

class Modify : public Action
{
public:
	int Execute();
	QString toModify;
};

class Step
{
public:
	int Run();
public:
	QString name;
	QVector<Action*> behavior;	
};

class VarList
{
public:
	int SIZE(int type);
public:
	//pair<Type, Pos>
	QHash<QString, QPair<int, int>> noteTable;
	QVector<int> val;
	QVector<QString> word;
};

class Interpreter
{
public:
	bool AnalysisScript(QString filePath);
	QVector<Step>& StepList();
	int Start();

private:
	bool GenerateStep(Step& step_to_create, QTextStream& qstream);
	bool SpeakProcess(Step& step_to_modify, QStringList& buffer);
	bool BranchProcess(Step& step_to_modify, QStringList& buffer);
	bool ListenProcess(Step& step_to_modify, QStringList& buffer);
	bool SilenceProcess(Step& step_to_modify, QStringList& buffer);
	bool ExitProcess(Step& step_to_modify, QStringList& buffer);
	bool ModifyProcess(Step& step_to_modify, QStringList& buffer);
	bool DefaultProcess(Step& step_to_modify, QStringList& buffer);


private:
	VarList var;
	QVector<Step> stepList;
	QHash<QString, int> stepPos;
	QString entry;
};




#endif // !INTERPRETER_H
