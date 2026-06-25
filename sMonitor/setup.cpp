#include "setup.h"
#include "ui_setup.h"

setup::setup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::setup)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(0, 0, 1024, 600);
    selectCategory(0);

    /* 四个预置位的固定角度 */
    m_presets[0].lr = 80;   m_presets[0].ud = 60;   /* 大门 */
    m_presets[1].lr = 70;   m_presets[1].ud = 75;   /* 仓库 */
    m_presets[2].lr = 85;   m_presets[2].ud = 65;   /* 停车场 */
    m_presets[3].lr = 65;   m_presets[3].ud = 70;   /* 围墙 */
    for (int i = 0; i < 4; i++)
        m_presets[i].enabled = false;

    m_cruiseIndex = 0;
    m_cruiseCount = 0;
    m_cruiseSpeed = 5;
    m_cruiseRunning = false;
    ui->toggleCruise->setChecked(false);

    connect(ui->speedSlider, SIGNAL(valueChanged(int)),
            this, SLOT(on_speedChanged(int)));

    /* 巡航开关：收到 checked 变化就启动/停止巡航 */
    connect(ui->toggleCruise, SIGNAL(clicked(bool)),
            this, SLOT(onToggleCruise(bool)));
}

setup::~setup()
{
    delete ui;
}

void setup::on_backBt_clicked() { this->close(); }

void setup::selectCategory(int index)
{
    clearSelection();
    ui->contentStack->setCurrentIndex(index);
    QStringList titles;
    titles << "巡航设置" << "告警策略配置" << "录像存储设置" <<
             "网络与设备管理" << "用户权限管理";
    ui->rightTitleText->setText(titles[index]);
    QString sel = "background: #e8f0fe; border: none; text-align: left;"
                  "font-size: 13px; font-weight: 500; color: #333333; padding-left: 44px;";
    QPushButton *b = findChild<QPushButton*>(QString("nav%1").arg(index + 1));
    if (b) b->setStyleSheet(sel);
}

void setup::clearSelection()
{
    QString n = "background: transparent; border: none; text-align: left;"
                "font-size: 13px; font-weight: 500; color: #333333; padding-left: 44px;";
    for (int i = 1; i <= 5; i++) {
        QPushButton *b = findChild<QPushButton*>(QString("nav%1").arg(i));
        if (b) b->setStyleSheet(n);
    }
}

void setup::on_nav1_clicked() { selectCategory(0); }
void setup::on_nav2_clicked() { selectCategory(1); }
void setup::on_nav3_clicked() { selectCategory(2); }
void setup::on_nav4_clicked() { selectCategory(3); }
void setup::on_nav5_clicked() { selectCategory(4); }
void setup::on_saveBt_clicked() { }
void setup::on_speedChanged(int val)
{
    ui->speedVal->setText(QString::number(val));
    m_cruiseSpeed = val;
    if (m_eng) m_eng->setCruiseSpeed(val);
}
void setup::on_resetBt_clicked() { }

/* ========== 预置位复选框：只标记，不移动 ========== */

void setup::on_ckDoor_clicked()
{
    m_presets[0].enabled = ui->ckDoor->isChecked();
    if (m_presets[0].enabled && m_eng)
        m_eng->gotoAngle(m_presets[0].lr, m_presets[0].ud, m_cruiseSpeed);
    /* 如果巡航开关是开的但巡航没在跑 → 启动 */
    if (ui->toggleCruise->isChecked() && !m_cruiseRunning)
        startCruise();
}

void setup::on_ckWarehouse_clicked()
{
    m_presets[1].enabled = ui->ckWarehouse->isChecked();
    if (m_presets[1].enabled && m_eng)
        m_eng->gotoAngle(m_presets[1].lr, m_presets[1].ud, m_cruiseSpeed);
    if (ui->toggleCruise->isChecked() && !m_cruiseRunning)
        startCruise();
}

void setup::on_ckParking_clicked()
{
    m_presets[2].enabled = ui->ckParking->isChecked();
    if (m_presets[2].enabled && m_eng)
        m_eng->gotoAngle(m_presets[2].lr, m_presets[2].ud, m_cruiseSpeed);
    if (ui->toggleCruise->isChecked() && !m_cruiseRunning)
        startCruise();
}

void setup::on_ckWall_clicked()
{
    m_presets[3].enabled = ui->ckWall->isChecked();
    if (m_presets[3].enabled && m_eng)
        m_eng->gotoAngle(m_presets[3].lr, m_presets[3].ud, m_cruiseSpeed);
    if (ui->toggleCruise->isChecked() && !m_cruiseRunning)
        startCruise();
}

/* ========== 巡航控制 ========== */

void setup::onToggleCruise(bool checked)
{
    if (checked) startCruise();
    else stopCruise();
}

void setup::startCruise()
{
    int lrs[4], uds[4], count = 0;
    for (int i = 0; i < 4; i++) {
        if (m_presets[i].enabled) {
            lrs[count] = m_presets[i].lr;
            uds[count] = m_presets[i].ud;
            count++;
        }
    }
    if (count < 2) return;

    m_cruiseRunning = true;
    m_cruiseCount = count;
    m_cruiseIndex = 0;

    /* 连接 cruiseDone 信号 */
    disconnect(m_eng, SIGNAL(cruiseDone()), this, SLOT(onCruiseTick()));
    connect(m_eng, SIGNAL(cruiseDone()), this, SLOT(onCruiseTick()));

    /* 走到第一个预置位，到达后会发 cruiseDone 信号 */
    m_eng->startCruise(lrs, uds, count, m_cruiseSpeed);
}



void setup::stopCruise()
{
    m_cruiseRunning = false;
    disconnect(m_eng, SIGNAL(cruiseDone()), this, SLOT(onCruiseTick()));
    m_eng->setCruiseEnabled(false);
    m_eng->stop();
}

void setup::onCruiseTick()
{
    if (!m_cruiseRunning || !m_eng) return;

    /* 收集当前打勾的预置位 */
    int lrs[4], uds[4], count = 0;
    for (int i = 0; i < 4; i++) {
        if (m_presets[i].enabled) {
            lrs[count] = m_presets[i].lr;
            uds[count] = m_presets[i].ud;
            count++;
        }
    }
    if (count < 2) { stopCruise(); return; }

    /* 如果 m_cruiseIndex 超出范围则回零 */
    if (m_cruiseIndex >= count) m_cruiseIndex = 0;

    /* 检查是否到达当前目标 */
    int curLr = m_eng->getLrAngle();
    int curUd = m_eng->getUdAngle();
    int targetLr = lrs[m_cruiseIndex];
    int targetUd = uds[m_cruiseIndex];

    if (abs(curLr - targetLr) <= 1 && abs(curUd - targetUd) <= 1) {
        /* 到达目标，切到下一个预置位 */
        m_cruiseIndex++;
        if (m_cruiseIndex >= count) m_cruiseIndex = 0;
        m_eng->gotoAngle(lrs[m_cruiseIndex], uds[m_cruiseIndex], m_cruiseSpeed);
    }
}
