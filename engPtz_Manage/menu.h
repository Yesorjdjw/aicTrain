#ifndef MYENG_H
#define MYENG_H

#include <QMainWindow>
#include "steng.h"
#include <QDebug>

namespace Ui {
class menu;
}

class menu : public QMainWindow
{
    Q_OBJECT

public:
    explicit menu(QWidget *parent = 0);
    ~menu();

private slots:


private:
    Ui::menu *ui;

};

#endif // MYENG_H






















