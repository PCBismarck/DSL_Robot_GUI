// DSL_Robot.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once
#include <QtConcurrent/QtConcurrent>
#include "Action.h"
#include "Interpreter.h"


//客服机器人类
class DSL_Robot : public QObject
{
public:
	//与客户端通信的逻辑
	void StartWorking(qintptr socketDescriptor, Interpreter* SynTree);
	//接收到信号突然断开后的应对操作
	void HandleDisconnect();
	//复制初始变量表
	void copy_varList(VarList* to_copy);
public:
	//Robot自己维护的变量表
	VarList MyVal;
};

// TODO: 在此处引用程序需要的其他标头。
