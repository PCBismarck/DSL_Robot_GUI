#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_DSL_Robot_GUI.h"

class DSL_Robot_GUI : public QMainWindow
{
    Q_OBJECT

public:
    DSL_Robot_GUI(QWidget *parent = nullptr);
    ~DSL_Robot_GUI();

private:
    Ui::DSL_Robot_GUIClass ui;
};