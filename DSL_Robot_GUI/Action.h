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
#include <QTcpSocket>
#include <QThread>
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
	virtual int Execute(QString& OBuffer, 
		VarList& var, 
		QTcpSocket* client) = 0;
};

class Speak :public Action
{
public:
	int Execute(QString &OBuffer, VarList& var, QTcpSocket* client);
public:
	QVector<QString> words;
};

class Branch : public Action
{
public:
	int Execute(QString &OBuffer, VarList& var, QTcpSocket* client);
public:
	QHash<QString, QString> jump;
};

class Listen : public Action
{
	//设置timer
public:
	int Execute(QString &OBuffer, VarList& var, QTcpSocket* client);
public:
	int waitTime;
	int heardType;
};

class Exit : public Action
{
public:
	int Execute(QString &OBuffer, VarList& var, QTcpSocket* client);
};

class Silence : public Action
{
public:
	int Execute(QString &OBuffer, VarList& var, QTcpSocket* client);
public:
	QString jumpTo;
};

class Default : public Action
{
public:
	int Execute(QString &OBuffer, VarList& var, QTcpSocket* client);
public:
	QString jumpTo;
};

class Modify : public Action
{
public:
	int Execute(QString &OBuffer, VarList& var, QTcpSocket* client);
public:
	QString toModify;
	QVector<QString> varQue;
	QVector<QString> opQue;
};

class Step
{
public:
	int Run(QString& SOBuffer, VarList& var, QTcpSocket* client);
public:
	QString name;
	QVector<Action*> behavior;

};

#endif
