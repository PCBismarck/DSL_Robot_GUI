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