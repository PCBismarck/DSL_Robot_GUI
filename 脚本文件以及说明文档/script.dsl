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


