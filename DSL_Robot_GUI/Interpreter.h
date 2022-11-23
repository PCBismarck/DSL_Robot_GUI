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
	bool AnalysisScript(QString filePath);
	QVector<Step>& StepList();
	//int Start(QStringList& IBuffer, 
	//	QTcpSocket* client);

private:
	bool GenerateStep(Step& step_to_create, QTextStream& qstream);
	bool SpeakProcess(Step& step_to_modify, QStringList& buffer);
	bool BranchProcess(Step& step_to_modify, QStringList& buffer);
	bool ListenProcess(Step& step_to_modify, QStringList& buffer);
	bool SilenceProcess(Step& step_to_modify, QStringList& buffer);
	bool ExitProcess(Step& step_to_modify, QStringList& buffer);
	bool ModifyProcess(Step& step_to_modify, QStringList& buffer);
	bool DefaultProcess(Step& step_to_modify, QStringList& buffer);


public:
	VarList var;
	QVector<Step> stepList;
	QHash<QString, int> stepPos;
	QString entry;
};

//extern Interpreter SynTree;


#endif // !INTERPRETER_H
