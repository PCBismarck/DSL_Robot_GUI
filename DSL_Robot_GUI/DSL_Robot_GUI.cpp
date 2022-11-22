#include "DSL_Robot_GUI.h"

DSL_Robot_GUI::DSL_Robot_GUI(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    Interpreter MyInter;
    //MyInter.AnalysisScript()
}

DSL_Robot_GUI::~DSL_Robot_GUI()
{}

void DSL_Robot_GUI::on_sendButton_clicked()
{
    QString msg = ui.textEdit->toPlainText();
    ui.textEdit->setText("");
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    if (!msg.isEmpty())
    {
		ui.listWidget->addItem(time);
		ui.listWidget->addItem(msg);
    }

}
