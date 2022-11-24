#include "DSL_Robot_GUI.h"
#include <QtWidgets/QApplication>
#include "Interpreter.h"
#include "Action.h"
#include <QThread>
#include <QTcpSocket>
#include <QTcpServer>
#include <QHostAddress>
#include <QMessageBox>
#include "Action.h"

using namespace std;

//#define _TEST_
//#define _TEST_NET_

int main(int argc, char *argv[])
{

#ifdef _TEST_
	QString path = "E:/desktop/DSL-Robot/script.dsl";
	//"E:/desktop/DSL-Robot/testGrammar.dsl";
#endif // !_TEST_

#ifdef _TEST_NET_

#endif // !_TEST_NET_

#ifndef _TEST_
	if (argc <= 1)
		return -114514;
	QString path = QString(argv[1]);
#endif


	QApplication a(argc, argv);
	DSL_Robot_GUI w(path);
	
	//w.show();
	return a.exec();
}
