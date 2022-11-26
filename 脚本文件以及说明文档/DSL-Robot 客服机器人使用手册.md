# <center>DSL-Robot 客服机器人使用手册









## 0. 机器人简介

该客服机器人可以根据给定的脚本文件生成一个状态转移自动机，机器人根据生成的自动机进行状态转换给出应答回复，从而实现简单的客服机器人应答功能。

脚本文件在服务端部署后，可以使用客户端访问服务器进行查询

支持多人同时查询

![image-20221124143135966](C:\Users\28145\AppData\Roaming\Typora\typora-user-images\image-20221124143135966.png)





<div STYLE="page-break-after: always;"></div>

效果演示如下：

<img src="C:\Users\28145\AppData\Roaming\Typora\typora-user-images\image-20221124140537933.png" alt="image-20221124140537933" style="zoom: 60%;" /><img src="C:\Users\28145\AppData\Roaming\Typora\typora-user-images\image-20221124140553282.png" alt="image-20221124140553282" style="zoom: 60%;" />





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



## 2 脚本语法说明

脚本文件主要分为两部分：

> 1. 预定义部分
> 2. 状态定义部分

### 2.1 预定义部分

预定义部分用来定义在程序中需要用到的变量以及设定程序的入口，主要有以下两个关键字：

> 1. [Entry](#Entry)
> 2. [Set](#Set)



<span id="Entry">**Entry**</span>用来设置程序的主入口，

> 
> 语法为:
> 
> ```
> Entry StepName
> ```
> 
> 近似于c语言中的main函数，例如样例脚本中的：
> 
> ```
> Entry welcome
> 
> Step welcome 
> 	Speak $name+"您好，请问有什么可以帮您?"
> 	Listen Word 10
> 	Branch "投诉":complainProc	"账单":billProc "充值":chargeProc
> 	Silence silenceProc
> 	Default thanks
> 	End
> ```
> 
> 通过Entry关键字将**welcome**这个**Step**设置为了程序的开始状态，你同样可以将其他的> Step设置为开始状态，客服机器人将会从这个状态开始运行。
> 
> **注意：每个脚本文件都应该设置有Entry，如果设置多个Entry将会按照最后设置的作为最终的> Entry**



<span id="Set">**Set**</span>用来设置程序中使用到的变量，语法规则如下
>
>```
>Set Type TypeName value
>```
>
>**Type**即变量类型，脚本支持两种类型的变量：
>
>​	1. **Word** : 字符串类型的变量
>
>​	2. **Var** : 整形变量（暂不支持浮点数）
>
>**TypeName**即之后状态中用到的变量的名称 
>
>​	**注意**：脚本中自定义的变量名**不能设置为Heard**，**Heard**是Listen关键字维护的>系统变量，在Listen语句执行之后使用
>
>**value**即为变量设置的初值，需要注意：**变量必须赋初值**
>
>由以上 **4** 个词构成一条完整的变量设置语句，**3个参数缺一不可**
>
>在使用定义好的变量时，需要在变量名前加上 **$** 标识变量，例如：
>
>```
>Set Word name "张三"
>Set Var amount 0 
>···
>step and sentence
>···
>Speak $name+"您好，请问有什么可以帮您?"
>Speak "您的本月账单是"+$amount+"元，感谢您的来电，再见"
>```
>
>第6、7行的语句在执行后将会输出
>
>> 张三您好，请问有什么可以帮您？
>
>> 您的本月账单是0元，感谢您的来电，再见



### 2.2 状态定义部分

> 转态定义主要有以下关键字：
>1. [Step](#Step)
>2. [Speak](#Speak)
>3. [Listen](#Listen)
>4. [Silence](#Silence)
>5. [Branch](#Branch)
>6. [Modify](#Modify)
>7. [Default](#Default)
>8. [End](#End)



状态声明样例：
>
>```
>Step Hello
>	Speak "Hello! welcome to use this robot!"
>	Exit
>	End
>```
>
>首一行就是一个语句，每一行都应该由一个关键字以及其对应的参数组成



<span id="Step">**Step**</span> 状态声明关键字

>
>   语法：
>
>```
>Step StepName
>```
>
>​	用**Step**声明一个语句之后，直到该Step后的第一个**End**之间的所有语句都属于声明的>**Step**的状态（不允许嵌套声明Step），由这些语句构成Step中执行的动作
>
>Step中的动作会顺序执行，但可以使用**Branch、Silence、Default**等关键字实现状态的跳转
>
>



<span id="Speak">**Speak**</span>关键字

>语法：
>
>```
>Speak Sentence   "您的本月账单是"+$amount+"元，感谢您的来电，再见"
>```
>
>Sentence应该是一个完整的句子，在中间引入变量则需要用 **+** 进行连接
>
>例如：
>
>```
>Speak $name+"您好，请问有什么可以帮您?"
>Speak "您的本月账单是"+$amount+"元，感谢您的来电，再见"
>```
>
>Speak语句是向用户进行输出的**唯一方式**





<span id="Listen">**Listen**</span>关键字

>语法:
>
>```
>Listen Type ListenTime
>```
>
>Listen关键字是获取用户输入的方式，Listen会维护一个全局的变量 **Heard** 
>
>Type 支持 **Word** 和 **Var** 两种
>
>ListenTime 表示最长可以接受的等待时间，超时则会设置**Silence**标志
>
>例如对于语句：
>
>```
>Listen Word 10
>```
>
>即系统会监听用户的输入10s，如果10s内用户没有输入任何信息，则Listen阶段结束，并设置**Silence**标志(详见Silence关键字)
>
>如果监听到用户的输入，则根据设置的类型，将输入转为对应类型的数据存入 **Heard** 变量中
>
>例如 执行语句：
>
>```
>Listen Word 10
>```
>
>若用户在 10 s内输入以下内容：
>
>> 你的客服机器人也太棒了吧！
>
>则 **Heard** 变量中将会存储字符串： "你的客服机器人也太棒了吧！"
>
>如果此时执行的下一条语句是：
>
>```
>Speak $Heard
>```
>
>程序将会输出
>
>> 你的客服机器人也太棒了吧！
>



<span id="Silence">**Silence**</span>关键字

>用以在用户长时间没有回应时进行状态转移
>
>语法：
>
>```
>Silence NextStep
>```
>
>**注意**：Silence关键字需要在**Listen**之后，只有Listen语句执行后，用户在等待时间内没有输入，Listen语句设置了**Silence**标志后，Silence语句才可以执行，如果Listen语句监听到了用户的输入，则Listen不会执行



<span id="Branch">**Branch**</span>关键字

>用以根据用户输入的语句进行状态转移，语法：
>
>```
>Branch "keyword":NextStepName "keyword2":Step2 ···
>```
>
>Branch需要在Listen语句执行后(Listen 语句中需要将 Type 设置为 Word类型)使用，与Silence语句不同的是，如果Listen语句设置了**Silence**标志，则Branch语句会直接跳过，按顺序执行后面的语句
>
>Branch语句会根据**Heard**(详见Listen关键字)变量顺序判断其中是否包含**keyword** 
>
>例如，对于语句：
>
>```
>Listen Word 20
>Branch "投诉":complainProc	"账单":billProc "充值":chargeProc
>```
>
>如果20s内用户输入了 
>
>> 我要充值
>
>则会分别对 "投诉"、 "账单"、 "充值" 三个关键字顺序进行判断，所以上面的语句会跳转 chargeProc 状态
>
>如果用户的输入为
>
>> 我要充值账单
>
>由于"账单" 关键字的检索顺序在 "充值" 之前，因而会跳转 billProc 状态
>



<span id="Modify">**Modify**</span>关键字

>
>Modify关键字是用以修改变量的，语法为：
>
>```
>Modify $VarToModify : Var1 Opperator1 Var2 Opperator2 Var3 ···
>```
>
>VarToModify必须是一个变量，根据变量的类型，支持的操作符也不一样，所有的**Var**都必须与VarToModify**类型一致**
>
>对于 Var类型，支持 + - * / 四种操作
>
>对于Word类型，支持 + 操作(即支持字符串的拼接)
>
>样例1：
>
>```
>Set Var Balance 0
>······
>Listen Var 20
>······
>Modify $Blance : $Balance + $Heard
>```
>
>这个语句可以完成将用户输入的数字(整形)加到Balance变量中
>
>样例2：
>
>```
>Set Word name "先生"
>······
>Listen Word 20
>······
>Modify $name : $Heard + $name
>```
>
>这个语句可以完成将用户输入的称呼加上"先生"称呼并存到name变量中



<span id="Default">**Default**</span>关键字

>无条件跳转语句，语法：
>
>```
>Default NextStep
>```
>
>无条件跳转到NextStep状态中，不会受到**Silence**标志的影响
>



<span id="End">**End**</span>关键字

>
>标志一个**Step**的结束，语法如下：
>
>```
>Step StepName
>	···
>	sentence
>	···
>	End
>```



## 3. 关键字索引

关键字索引

1. [Branch](#Branch)
2. [Default](#Default)
3. [End](#End)
4. [Entry](#Entry)
5. [Listen](#Listen)
6. [Modify](#Modify)
7. [Set](#Set)
8. [Silence](#Silence)
9. [Speak](#Speak)
10. [Step](#Step)



## 4. 演示中的样例脚本

```
Entry welcome
Set Word name 张三
Set Var balance 0

Step welcome
	Speak "您好欢迎使用客服查询机器人，可以告诉我您的名字吗?"
	Listen Word 20
	Silence silenceProc
	Modify $name : $Heard
	Default businessProc
	End
	
Step silenceProc
	Speak "可以请您告诉我该如何称呼您吗?"
	Listen Word 20
	Silence timeoutProc
	Modify $name : $Heard
	Default businessProc
	End

Step businessProc
	Speak "客服这边可以帮您查询余额、充值余额，也可以接受您的投诉意见 "
	Speak $name+"，请问有什么我可以帮您的吗? "
	Listen Word 20
	Silence businessProc_Silence
	Branch "查询":quiryProc "余额":quiryProc "充值":chargeProc 
	Branch "投诉":complainProc "建议":complainProc "意见":complainProc
	Default thanks
	End
	
Step thanks
	Speak "感谢你的对我们服务的支持，再见! "
	Exit
	End
	
Step timeoutProc
	Speak "您已长时间未回应，客服将结束会话"
	Exit
	End

Step quiryProc
	Speak "您的余额为"+$balance+"元"
	Default businessProc
	End

Step chargeProc
	Speak "请输入您希望充值的金额"
	Listen Var 10
	Modify $balance : $balance + $Heard
	Speak "您现在的余额为："+$balance+"元"
	Default businessProc
	End
	
Step complainProc
	Listen Word 10
	Speak "感谢您的反馈，我们以后一定会做得更好! "
	Default thanks
	End
	
Step businessProc_Silence
	Speak "您还有什么需要办理的吗? "
	Listen Word 10
	Silence thanks
	Branch "查询":quiryProc "余额":quiryProc "充值":chargeProc 
	Branch "投诉":complainProc "建议":complainProc "意见":complainProc
	Default thanks
	End

```
