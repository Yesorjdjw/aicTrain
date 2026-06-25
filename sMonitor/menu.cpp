#include "unusual.h"
#include "setup.h"
#include "menu.h"
#include "ui_menu.h"

bool swt_Control_flag = true;

menu::menu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::menu)
{
    ui->setupUi(this);

    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/images/menu.jpg")));
    this->setPalette(palette);

    /* 统一初始状态：显示手动控制面板（PTZ 按钮） */
    swt_Control_flag = false;
    ui->swt_Control_bt->setText("切换自动");
    ui->ctl_Panel_sw->setCurrentIndex(0);

    /* setEng 构造中自动执行开机复位 backLrpos() + backUdpos() */
}

menu::~menu()
{
    delete ui;
}

/* ========== 导航按钮 ========== */

void menu::on_swt_Control_bt_clicked()
{
    if (swt_Control_flag) {
        swt_Control_flag = false;
        ui->swt_Control_bt->setText("切换手动");
        ui->ctl_Panel_sw->setCurrentIndex(1);
    } else {
        swt_Control_flag = true;
        ui->swt_Control_bt->setText("切换自动");
        ui->ctl_Panel_sw->setCurrentIndex(0);
    }
}

void menu::on_exp_Check_bt_clicked()
{
    unusual *un = new unusual();
    un->show();
}

void menu::on_file_Manage_bt_clicked()
{
    fops *fo = new fops();
    fo->show();
}

void menu::on_mode_Setup_bt_clicked()
{
    setup *st = new setup();
    st->setEngPtr(&sEng);
    st->show();
}

/* ========== PTZ 云台 — 按住持续移动，松开停止 ========== */

void menu::on_turn_up_bt_pressed()
{
    sEng.turnUp();
}

void menu::on_turn_up_bt_released()
{
    sEng.stop();
}

void menu::on_turn_down_bt_pressed()
{
    sEng.turnDown();
}

void menu::on_turn_down_bt_released()
{
    sEng.stop();
}

void menu::on_turn_left_bt_pressed()
{
    sEng.turnLeft();
}

void menu::on_turn_left_bt_released()
{
    sEng.stop();
}

void menu::on_turn_right_bt_pressed()
{
    sEng.turnRight();
}

void menu::on_turn_right_bt_released()
{
    sEng.stop();
}

void menu::on_turn_reset_bt_clicked()
{
    sEng.backLrpos();
    sEng.backUdpos();
}


void menu::on_get_photo_bt_pressed()
{
    /* 预留：拍照功能 */
}
