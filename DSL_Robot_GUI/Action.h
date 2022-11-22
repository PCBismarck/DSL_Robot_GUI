#pragma once

#ifndef ACTION_H
#define ACTION_H

#include <QVector>
#include <QString>
#include <utility>
#include <QHash>
#include <QDebug>
#include <QFile>
#include <QStack>
#include <QRegularExpression>
#include <QQueue>
//#include "Interpreter.h"

#define VAR 0
#define WORD 1

#define EXIT -5
#define EROR -1
#define NORM 1
#define JUMP 2
#define HEAR 5


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

class Action
{
public:
	//返回值表示不同的状态
	//
	virtual int Execute() = 0;
public:
	QStringList IBuffer;
	QStringList OBuffer;
	VarList* var;
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
public:
	QString toModify;
	QQueue<QString> varQue;
	QQueue<QString> opQue;
};

class Step
{
public:
	int Run();
public:
	QString name;
	QVector<Action*> behavior;
	VarList* var;
	QStringList SIBuffer;
	QStringList SOBuffer;
};

#endif
