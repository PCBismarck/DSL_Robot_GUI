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

//变量列表，存储脚本文件中设置的变量
class VarList
{
public:
	int SIZE(int type);
public:
	//QString -> <Type, pos>的对照表，根据变量名可以获得变量的信息，分别是变量的类型和变量在变量表中的位置
	//pair<Type, Pos>
	QHash<QString, QPair<int, int>> noteTable;    
	//Var类型的变量表
	QVector<int> val;
	//Word类型的变量表
	QVector<QString> word;
};

class Action
{
public:
	//返回值表示不同的状态
	//所有动作共有的动作执行函数
	//OBuffer是与上一层交换信息的缓冲区
	//var是该Robot维护的变量表
	//client是Tcp套接字
	virtual int Execute(QString& OBuffer, 
		VarList& var, 
		QTcpSocket* client) = 0;
};

class Speak :public Action
{
public:
	int Execute(QString &OBuffer, VarList& var, QTcpSocket* client);
public:
	//Speak中要输出的词汇
	QVector<QString> words;
};

class Branch : public Action
{
public:
	int Execute(QString &OBuffer, VarList& var, QTcpSocket* client);
public:
	//keyword -> nextStepName 对照表
	QHash<QString, QString> jump;
};

class Listen : public Action
{
	//设置timer
public:
	int Execute(QString &OBuffer, VarList& var, QTcpSocket* client);
public:
	//等待时间，超时在OBuffer中设置Silence标志
	int waitTime;
	//Heard变量的类型
	int heardType;
};

class Exit : public Action
{
public:
	//Execute直接返回EXIT，标志着Exit动作已经执行
	int Execute(QString &OBuffer, VarList& var, QTcpSocket* client);
};

class Silence : public Action
{
public:	
	//将OBuffer设置为JumpTo，并返回JUMP
	int Execute(QString &OBuffer, VarList& var, QTcpSocket* client);
public:
	//下一步跳转的Step名称
	QString jumpTo;
};

class Default : public Action
{
public:
	//将OBuffer设置为JumpTo，并返回JUMP
	int Execute(QString &OBuffer, VarList& var, QTcpSocket* client);
public:
	//下一步跳转的Step名称
	QString jumpTo;
};

class Modify : public Action
{
public:
	int Execute(QString &OBuffer, VarList& var, QTcpSocket* client);
public:
	//表达式左边的值，即要修改的变量名
	QString toModify;
	//表达式右边等式中的值
	QVector<QString> varQue;
	//表达式右边出现的操作符
	QVector<QString> opQue;
};

class Step
{
public:
	//按顺序执行Step中的Action
	int Run(QString& SOBuffer, VarList& var, QTcpSocket* client);
public:
	//Step的名称
	QString name;
	//Step中的各个Action
	QVector<Action*> behavior;

};

#endif
