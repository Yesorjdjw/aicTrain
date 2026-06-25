#include "steng.h"
#include <stdlib.h>

setEng::setEng()
    : QObject()
    , m_fd(-1)
    , m_lr(LR_CENTER)
    , m_ud(UD_CENTER)
    , m_timer(new QTimer(this))
    , m_targetLr(LR_CENTER)
    , m_targetUd(UD_CENTER)
    , m_isCruise(false)
    , m_cruiseCount(0)
    , m_cruiseIdx(0)
    , m_cruiseTick(0)
    , m_cruiseThreshold(1)
    , m_dir(DirNone)
{
    m_fd = ::open(DEV_PATH, O_RDWR);
    if (m_fd < 0) {
        qWarning("setEng: open %s failed", DEV_PATH);
        return;
    }
    backLrpos();
    backUdpos();
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTick()));
}

setEng::~setEng()
{
    if (m_fd >= 0) {
        ioctl(m_fd, LR_CENTER, PWM2_CTL);
        ioctl(m_fd, UD_CENTER, PWM3_CTL);
        ::close(m_fd);
    }
}

void setEng::apply()
{
    if (m_fd < 0) return;
    ioctl(m_fd, (unsigned int)m_lr, PWM2_CTL);
    ioctl(m_fd, (unsigned int)m_ud, PWM3_CTL);
}

/* ========== PTZ 方向控制 ========== */

void setEng::turnLeft()   { m_dir = DirLeft;   m_isCruise = false; m_timer->start(80); }
void setEng::turnRight()  { m_dir = DirRight;  m_isCruise = false; m_timer->start(80); }
void setEng::turnUp()     { m_dir = DirUp;     m_isCruise = false; m_timer->start(80); }
void setEng::turnDown()   { m_dir = DirDown;   m_isCruise = false; m_timer->start(80); }
void setEng::stop()       { m_isCruise = false; m_timer->stop(); m_dir = DirNone; }

void setEng::backLrpos()  { m_timer->stop(); m_dir = DirNone; m_lr = LR_CENTER; apply(); }
void setEng::backUdpos()  { m_timer->stop(); m_dir = DirNone; m_ud = UD_CENTER; apply(); }

void setEng::setLrAngle(int ang) {
    if (ang < LR_MIN) ang = LR_MIN;
    if (ang > LR_MAX) ang = LR_MAX;
    m_lr = ang; apply();
}
int setEng::getLrAngle() { return m_lr; }
void setEng::setUdAngle(int ang) {
    if (ang < UD_MIN) ang = UD_MIN;
    if (ang > UD_MAX) ang = UD_MAX;
    m_ud = ang; apply();
}
int setEng::getUdAngle() { return m_ud; }

/* ========== 单次移动到目标 ========== */

void setEng::gotoAngle(int lr, int ud, int step)
{
    m_targetLr = lr;
    m_targetUd = ud;
    m_dir = DirNone;
    if (m_timer->isActive()) m_timer->stop();
    m_cruiseThreshold = 11 - step;
    m_cruiseTick = 0;
    m_timer->start(10);
}

/* ========== 巡航 ========== */

void setEng::startCruise(int lrs[], int uds[], int count, int step)
{
    if (count < 2) return;
    m_cruiseCount = count;
    m_cruiseIdx = 0;
    m_isCruise = true;
    m_dir = DirNone;
    for (int i = 0; i < count; i++) {
        m_cruiseLrs[i] = lrs[i];
        m_cruiseUds[i] = uds[i];
    }
    m_targetLr = m_cruiseLrs[0];
    m_targetUd = m_cruiseUds[0];
    if (m_timer->isActive()) m_timer->stop();
    m_cruiseThreshold = 11 - step;
    m_cruiseTick = 0;
    m_timer->start(10);
}

void setEng::setCruiseEnabled(bool on)
{
    m_isCruise = on;
}

void setEng::setCruiseSpeed(int step)
{
    if (m_isCruise) {
        m_cruiseThreshold = 11 - step;
    }
}

void setEng::stopCruise()
{
    m_isCruise = false;
    m_cruiseCount = 0;
    m_cruiseIdx = 0;
    m_timer->stop();
    m_dir = DirNone;
}

/* ========== 定时器 ========== */

void setEng::onTick()
{
    if (m_isCruise && m_cruiseCount > 0) {
        /* 固定 10ms 定时器，步数计数器控制速度 */
        m_cruiseTick++;
        if (m_cruiseTick < m_cruiseThreshold) return;
        m_cruiseTick = 0;
        /* 巡航模式：走向当前目标，到达后自动切下一个 */
        bool arrived = true;

        if (m_lr < m_targetLr) { m_lr += STEP; arrived = false; }
        else if (m_lr > m_targetLr) { m_lr -= STEP; arrived = false; }

        if (m_ud < m_targetUd) { m_ud += STEP; arrived = false; }
        else if (m_ud > m_targetUd) { m_ud -= STEP; arrived = false; }

        if (arrived) {
            m_cruiseIdx++;
            if (m_cruiseIdx >= m_cruiseCount) m_cruiseIdx = 0;
            m_targetLr = m_cruiseLrs[m_cruiseIdx];
            m_targetUd = m_cruiseUds[m_cruiseIdx];
        }
        apply();
        return;
    }

    if (m_dir != DirNone) {
        /* PTZ 方向控制 */
        switch (m_dir) {
        case DirLeft:  if (m_lr > LR_MIN) m_lr -= STEP; break;
        case DirRight: if (m_lr < LR_MAX) m_lr += STEP; break;
        case DirUp:    if (m_ud > UD_MIN) m_ud -= STEP; break;
        case DirDown:  if (m_ud < UD_MAX) m_ud += STEP; break;
        default: break;
        }
        apply();
        return;
    }

    /* gotoAngle 单次移动 */
    bool done = true;
    if (m_lr < m_targetLr) { m_lr += STEP; done = false; }
    if (m_lr > m_targetLr) { m_lr -= STEP; done = false; }
    if (m_ud < m_targetUd) { m_ud += STEP; done = false; }
    if (m_ud > m_targetUd) { m_ud -= STEP; done = false; }

    if (done) m_timer->stop();
    else apply();
}
