#include "DSL_Robot_GUI.h"
#include <QtWidgets/QApplication>
#include "Interpreter.h"
#include "Action.h"
#include <QThread>
#include <QTcpSocket>
#include <QTcpServer>
#include <QHostAddress>
#include "Action.h"

using namespace std;

//#define _TEST_INTER_
#define _TEST_NET_

int main(int argc, char *argv[])
{

#ifdef _TEST_INTER_
	Interpreter MyInter;
	QString filePath = "E:/desktop/DSL-Robot/testGrammar.dsl";
	MyInter.AnalysisScript(filePath);

	//MyInter.StepList()[0].behavior[0]->Execute();
	MyInter.Start();
	//MyInter.StepList()[0].Run();
	//MyInter.Start();
#endif // !_TEST_

#ifdef _TEST_NET_

#endif // !_TEST_NET_

	//VarList v1;
	//v1.val.push_back(1);
	//v1.word.push_back("FUCK");
	//VarList V2 = v1;
	//qDebug() << v1.val << V2.val;
	//V2.val[0] = 100;
	//qDebug() << v1.val << V2.val;
	//SynTree.AnalysisScript("E:/desktop/DSL-Robot/testGrammar.dsl");
	QApplication a(argc, argv);
	DSL_Robot_GUI w;
	//w.show();
	return a.exec();
}
