# DSL_Robot_GUI
用QT重构了原本的Robot，DSL客服机器人的GUI版本，打算做成类似QQ的人机交互界面
## 项目使用方法：
### 服务端
DSL_Robot_Server.sln是VS解决方案，DSL_Robot_GUI文件夹下是服务端的项目文件
编译生成后在命令行输入 
```DSL_Robot_Server scriptPath```
运行服务器
### 客户端
Robot_fore_end_GUI文件夹里面是客户端的Qt工程文件，用Qt打开CmakeLists.txt文件可以打开工程文件
编译成功后可以从客户端发起连接

## 0. 机器人简介

该客服机器人可以根据给定的脚本文件生成一个状态转移自动机，机器人根据生成的自动机进行状态转换给出应答回复，从而实现简单的客服机器人应答功能。

脚本文件在服务端部署后，可以使用客户端访问服务器进行查询

支持多人同时查询

![image-20221124143135966](https://raw.githubusercontent.com/PCBismarck/picturebed/main/img/image-20221124143135966.png)





<div STYLE="page-break-after: always;"></div>

效果演示如下：

<img src="https://raw.githubusercontent.com/PCBismarck/picturebed/main/img/image-20221124140537933.png" alt="image-20221124140537933" style="zoom: 60%;" /><img src="https://raw.githubusercontent.com/PCBismarck/picturebed/main/img/image-20221124140553282.png" alt="image-20221124140553282" style="zoom: 60%;" />





## 1. 快速开始

以下是一个功能较为全面的样例脚本：

```dsl
Set Var amount 0 
Set Word name "张三"
Entry welcome

Step welcome 
	Speak $name+"您好，请问有什么可以帮您?"
	Listen Word 10
	Branch "投诉":complainProc	"账单":billProc "充值":chargeProc
	Silence silenceProc
	Default thanks
	End

Step complainProc
	Speak "您的意见是我们改进工作的动力，请问您还有什么补充?"
	Listen Word 10
	Default thanks
	End

Step thanks
	Speak "感谢您的来电，再见"
	Exit
	End

Step billProc 
	Speak "您的本月账单是"+$amount+"元，感谢您的来电，再见"
	Exit
	End

Step silenceProc
	Speak "听不清，请您大声一点可以吗"
	Listen Word 10
	Branch "投诉":complainProc "账单":billProc "充值":chargeProc
	Default thanks
	End

Step chargeProc
	Speak "How much?"
	Listen Var 10
	Modify $amount : $amount+50
	Exit
	End
```

如示例脚本中展现的，您可以通过**Set**关键字设置变量并在状态中使用它，您可以通过**Step**和**End**设置不同的状态，在状态中设置各种顺序执行的操作，也可以通过**Branch、Silence、Default**等关键字进行状态的转移，满足更多语境的需要。
