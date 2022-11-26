# <center>DSLRobot客服机器人设计实现及测试说明

## I 整体结构说明

### 1. DSLRobot后端运行方式说明

![image-20221125134408163](C:\Users\28145\AppData\Roaming\Typora\typora-user-images\image-20221125134408163.png)

<center> 图 <b>1</b> 服务端主要类关系说明

服务端的运行流程大致如下：

1. DSL_Robot_GUI类创建Interpreter类，对给定的脚本文件进行分析并创建状态转换机和初始的变量表
2. 每当客户端发来连接请求，DSL_Robot_GUI将会创建一个DSL_Robot实例，并创造一个线程运行该Robot的StartWokring方法
3. 每个DSL_Robot都会从Interpreter中复制一份变量表由自己线程进行维护
4. 采用Tcp连接，每个线程的Robot实例负责一个客户
5. 所有Robot共用一个Interpreter的状态转换表

### 2. Interpreter执行结构说明

![image-20221125145033590](C:\Users\28145\AppData\Roaming\Typora\typora-user-images\image-20221125145033590.png)

<center> 图 <b>2</b> Interpreter包含的关系说明



## II 各类的详细说明

### 1. DSL_Robot_Server类的说明

```c++
//服务端的主要入口类，在其他线程中创建Robot回复客户
class DSL_Robot_Server : public QMainWindow
{
    Q_OBJECT

public:
    //在构造该类时需要给出一个脚本文件地址，在创建该类时同时创建对应脚本文件的Interpreter类
    DSL_Robot_GUI(QString ScriptPath, QWidget *parent = nullptr);
    ~DSL_Robot_GUI();
private slots:
    //接收到连接请求后开启一个新线程，并设置Robot
    void start_new_thread(qintptr socketDescriptor);

private:
    Ui::DSL_Robot_GUIClass ui;
    //Tcp Sever类
    TcpServerThr* server;
    //Interpreter类，解析脚本文件，生成状态机
    Interpreter SynTree;
};
```



### 2. Action及相关类

Action类是分别对应状态机中各种具体关键字动作的类，用以执行各种操作，由各种Action又构成一个Step类

```c++
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

//动作抽象基类
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

//Step类
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
```

Action类会直接与客户端进行数据通信(socket的read和write)，是信息交换的最小环节



### 3. Interpreter类

![image-20221125143628628](C:\Users\28145\AppData\Roaming\Typora\typora-user-images\image-20221125143628628.png)

<center>图 <b>3</b> Interpreter类属性




```c++
class Interpreter
{
public:
    //对给定的脚本文件进行分析，并创建状态机
	bool AnalysisScript(QString filePath);
    //返回StepList
	QVector<Step>& StepList();

    //以下函数用于分析、生成状态机
private:
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
```



### 4. DSL_Robot类说明

~~~c++
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
~~~



## III 测试

#### 1. 测试环境

- Visual Studio 2022 community

- MSVC2019_64

- Qt 6.3.2

- Qt VS Project Format Version 3.4

- Qt Modules: 

  - core;
  - gui;
  - network;
  - widgets;
  - concurrent

  

#### 2. 单元测试

这部分主要是开发过程中进行的，对于指定的脚本利用VS的Debug模式查看内存可以很方便的检测状态机是否正确生成

![image-20221125152144409](C:\Users\28145\AppData\Roaming\Typora\typora-user-images\image-20221125152144409.png)

<center>图 <b>4</b> 利用Debug工具查看状态建立情况



#### 3. 集成测试

为了更直观的测试客服机器人的运行情况，我设计实现了前端人机交互界面，可以直观的与客服机器人进行交互：

![image-20221125152510348](C:\Users\28145\AppData\Roaming\Typora\typora-user-images\image-20221125152510348.png)

<center>图 <b>5</b> 利用前端人机交互界面测试

由于根据测试脚本的不同，客服机器人的应答逻辑也大不相同，因此这部分主要是进行人工输入检测状态机的各种逻辑跳转是否正常



#### 4.  压力测试

为了检测服务端对于多用户查询情况下的正确性，我构造了一个测试桩程序

```c++
#include <QtCore/QCoreApplication>
#include <QTcpSocket>
#include <QtConcurrent>
#include <QThread>
//查询函数
int createNewConnection()
{
    int turn = 0;
    QTcpSocket client;
    client.connectToHost("127.0.0.1", 7777);
    QString msg;
    QString result;
	QString id = QString("%1").arg(quintptr(QThread::currentThreadId()));

	client.waitForReadyRead();
	msg = QString::fromLocal8Bit(client.readAll());
    qDebug() << id; //+ msg;
    msg = "大雄" + id;

    // 输入称呼
    QThread::sleep(1);
	client.waitForReadyRead();
	msg = QString::fromLocal8Bit(client.readAll());
    QThread::sleep(1);
    client.write(msg.toLocal8Bit());
    client.waitForBytesWritten();
     
    // 输入充值请求
    client.waitForReadyRead();
	QThread::sleep(1);
    client.write(QString("我要充值").toLocal8Bit());
    client.waitForBytesWritten();
    QThread::sleep(0.5);

    // 充值 id
    client.waitForReadyRead();
    QThread::sleep(1);
    client.write(QString(id).toLocal8Bit());
	client.waitForBytesWritten();
    QThread::sleep(0.5);

    // 输入查询
    client.waitForReadyRead();
    QThread::sleep(1);
    client.write(QString("查询").toLocal8Bit());
    client.waitForBytesWritten();
    QThread::sleep(0.5);

    // 输入结束
    client.waitForReadyRead();
    QThread::sleep(1);
	client.write(QString("再见").toLocal8Bit());
    client.waitForBytesWritten();
    QThread::sleep(0.5);

    msg = QString::fromLocal8Bit(client.readAll());
    result = msg.mid(msg.indexOf("：")+1, msg.indexOf("元") - msg.indexOf("：")-1); 
    if (id == result)
        return -1;
    return id.toInt();
}


int main(int argc, char *argv[])
{
    if (argc == 1)
        return -114514;
    int num = QString(argv[1]).toInt();
    QCoreApplication a(argc, argv);
    QList<QFuture<int>> fulist;
    for (int i = 0; i < num; ++i)
    {
        QFuture<int> fu = QtConcurrent::run(createNewConnection);
        fulist.push_back(fu);
    }
    for (auto& iter : fulist)
    {
        iter.waitForFinished();
    }
    int cnt = 0;
    int fault = 0;
	for (auto& iter : fulist)
	{
        if (iter.result() > 0)
        {
            qDebug() << iter.result();
            fault++;
        }
	}
    qDebug() << fault << "Fault in total";
    return 0;
}

```

该测试函数会向服务器按照指定顺序发送以下信息：

1. "大雄" + 线程tid
2. "我要充值"
3.  输入线程tid
4. "查询"
5. "再见"

该函数模拟了一次常见的充值查询业务行为，并在函数的结尾判断查询的金额是否等于本线程的tid，如果相等则返回-1，否则返回线程tid，而主程序会利用多线程反复调用该函数同时想服务器发送查询请求，最后统一输出查询结果与线程tid不一致的线程

![image-20221125153606628](C:\Users\28145\AppData\Roaming\Typora\typora-user-images\image-20221125153606628.png)

<center>图 <b>6</b> 利用测试程序进行测试

开启100个线程进行测试

测试结果：

![image-20221125153719525](C:\Users\28145\AppData\Roaming\Typora\typora-user-images\image-20221125153719525.png)

<center>图 <b>7</b> 压力测试结果	

测试结果全部通过，说明每个线程的变量信息确实是相互独立的，且所有线程共用一个状态机转移表也是可行的

![image-20221125154137093](C:\Users\28145\AppData\Roaming\Typora\typora-user-images\image-20221125154137093.png)

<center>图 <b>8</b> 测试主机CPU信息

​	



#### 5. 测试总结

在压力测试中发现，虽然设置了可以开启100个用户，实际上最多同时进行16个通信，因为我是通过多线程的方式进行测试的，而我的CPU最多支持16个线程，因此实际上只能测试到16个用户同时请求的情况，如果出现了更多的用户，通信请求就会被阻塞，等到有空闲的线程才能继续执行；而我的服务器也是基于线程创建Robot进行一对一回复，理论上也只能支持最多16位客户同时查询，如果要增加更多的并发程序，那就需要改变通信协议，采用UDP协议，每个线程不再单独为某一位客户进行服务，改为每个客户端分发身份验证用户的状态单独进行存储，机器人根据客户身份信息结合对应的状态情况进行信息回复，并在内存区中在创建一份 身份<—>变量表 对照表用以查询。



