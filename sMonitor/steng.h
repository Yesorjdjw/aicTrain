#ifndef ENG_H
#define ENG_H

#include <QObject>
#include <QTimer>
#include <QDebug>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DEV_PATH  "/dev/sm_stENG"
#define PWM2_CTL  0x2
#define PWM3_CTL  0x3

#define LR_MIN    25
#define LR_MAX    125
#define LR_CENTER 75

#define UD_MIN    17
#define UD_MAX    117
#define UD_CENTER 67

#define STEP      1

class setEng : public QObject
{
    Q_OBJECT

public:
    setEng();
    ~setEng();

    void turnLeft();
    void turnRight();
    void turnUp();
    void turnDown();
    void stop();
    void setCruiseEnabled(bool on);
    void backLrpos();
    void backUdpos();
    void setLrAngle(int ang);
    int  getLrAngle();
    void setUdAngle(int ang);
    int  getUdAngle();
    void gotoAngle(int lr, int ud, int step);
    void startCruise(int lrs[], int uds[], int count, int step);
    void setCruiseSpeed(int step);
    void stopCruise();

signals:
    void cruiseDone();

private slots:
    void onTick();

private:
    enum Dir { DirNone, DirLeft, DirRight, DirUp, DirDown };

    int     m_fd;
    int     m_lr;
    int     m_ud;
    QTimer *m_timer;
    int     m_targetLr;
    int     m_targetUd;
    bool    m_isCruise;    // true=巡航自动切, false=单次停
    int     m_cruiseLrs[4];
    int     m_cruiseUds[4];
    int     m_cruiseCount;
    int     m_cruiseIdx;
    int     m_cruiseTick;
    int     m_cruiseThreshold;
    Dir     m_dir;

    void apply();
};

#endif // ENG_H
