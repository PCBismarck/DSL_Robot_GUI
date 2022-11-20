#include "DSL_Robot_GUI.h"
#include <QtWidgets/QApplication>
#include "Interpreter.h"

using namespace std;

int main(int argc, char *argv[])
{
	Interpreter MyInter;
	QString filePath = "E:/desktop/DSL-Robot/testGrammar.dsl";
	MyInter.AnalysisScript(filePath);
	MyInter.StepList()[0].behavior[0]->Execute();
	MyInter.StepList()[0].Run();
	MyInter.Start();
    QApplication a(argc, argv);
    DSL_Robot_GUI w;
    w.show();
    return a.exec();
}
