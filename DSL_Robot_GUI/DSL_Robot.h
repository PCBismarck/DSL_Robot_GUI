// DSL_Robot.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once
#include <QtConcurrent/QtConcurrent>
#include "Action.h"
#include "Interpreter.h"


class DSL_Robot : public QObject
{
public:
	void StartWorking(qintptr socketDescriptor, Interpreter* SynTree);
	void HandleDisconnect();
	void copy_varList(VarList* to_copy);
public:
	VarList MyVal;
};

// TODO: 在此处引用程序需要的其他标头。
