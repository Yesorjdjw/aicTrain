#ifndef ENG_H
#define ENG_H
#include <QObject>
#include <QTimer>
#include <QDebug>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>


class setEng : public QObject
{
    Q_OBJECT
public:
    setEng();
    void turnLeft();
    void turnRight();
    void setLrAngle(int ang);
    int getLrAngle();

    void turnUp();
    void turnDown();
    void setUdAngle(int ang);
    int getUdAngle();

    void backLrpos();
    void backUdpos();


public slots:

private:


};

#endif // ENG_H





























