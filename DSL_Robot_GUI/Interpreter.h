#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <QVector>
#include <QString>
#include <utility>
#include <QHash>
#include <QDebug>
#include <QFile>
#include <QStack>
#include <QRegularExpression>
#include <QTcpSocket>
#include "Action.h"


class Interpreter
{
public:
	~Interpreter();
	//对给定的脚本文件进行分析，并创建状态机
	bool AnalysisScript(QString filePath);
	//返回StepList
	QVector<Step>& StepList();

private:
	//以下函数用于分析、生成状态机
	//产生Step对象
	bool GenerateStep(Step& step_to_create, QTextStream& qstream);

	//生成对应的Action子类实例，并添加到step_to_modify的stepList中
	//buffer中是从脚本文件分析中得到的token
	bool SpeakProcess(Step& step_to_modify, QStringList& buffer);
	bool BranchProcess(Step& step_to_modify, QStringList& buffer);
	bool ListenProcess(Step& step_to_modify, QStringList& buffer);
	bool SilenceProcess(Step& step_to_modify, QStringList& buffer);
	bool ExitProcess(Step& step_to_modify, QStringList& buffer);
	bool ModifyProcess(Step& step_to_modify, QStringList& buffer);
	bool DefaultProcess(Step& step_to_modify, QStringList& buffer);


public:
	//变量列表
	VarList var;
	//状态列表
	QVector<Step> stepList;
	//StepName -> StepPos 对照表，记录某一Step在stepList中的位置
	QHash<QString, int> stepPos;
	//入口的Step名
	QString entry;
};



#endif // !INTERPRETER_H
