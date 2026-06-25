#include "steng.h"
#include "ui_menu.h"
#include "menu.h"

menu::menu(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::menu)
{
    ui->setupUi(this);

    //调整初始化角度

}

menu::~menu()
{
    delete ui;
}


