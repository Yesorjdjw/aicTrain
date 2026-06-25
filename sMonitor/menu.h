#ifndef MENU_H
#define MENU_H

#include <QWidget>
#include <fops.h>
#include "steng.h"

namespace Ui {
class menu;
}

class menu : public QWidget
{
    Q_OBJECT

public:
    explicit menu(QWidget *parent = 0);
    ~menu();

private slots:
    /* 导航按钮 */
    void on_swt_Control_bt_clicked();
    void on_exp_Check_bt_clicked();
    void on_file_Manage_bt_clicked();
    void on_mode_Setup_bt_clicked();

    /* PTZ 云台 — 按住持续移动，松开停止 */
    void on_turn_up_bt_pressed();
    void on_turn_up_bt_released();
    void on_turn_down_bt_pressed();
    void on_turn_down_bt_released();
    void on_turn_left_bt_pressed();
    void on_turn_left_bt_released();
    void on_turn_right_bt_pressed();
    void on_turn_right_bt_released();
    void on_turn_reset_bt_clicked();

    void on_get_photo_bt_pressed();

private:
    Ui::menu *ui;
    setEng sEng;
};

#endif // MENU_H
