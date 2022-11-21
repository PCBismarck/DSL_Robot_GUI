#include "DSL_Robot_GUI.h"
#include <QtWidgets/QApplication>
#include "Interpreter.h"
#include "Action.h"

using namespace std;

#define _TEST_

int main(int argc, char *argv[])
{

#ifdef _TEST_
	Interpreter MyInter;
	QString filePath = "E:/desktop/DSL-Robot/testGrammar.dsl";
	MyInter.AnalysisScript(filePath);
	//MyInter.StepList()[0].behavior[0]->Execute();
	MyInter.Start();
	//MyInter.StepList()[0].Run();
	//MyInter.Start();
#endif // !_TEST_
	//QHash<QString, QPair<int, int>> map;
	//qDebug() << map;
	//QPair<int, int> result = map["WHY"];
	//qDebug() << map;
    QApplication a(argc, argv);
    DSL_Robot_GUI w;
    w.show();
    return a.exec();
}
