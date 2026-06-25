#ifndef SETUP_H
#define SETUP_H

#include <QWidget>
#include "steng.h"
#include <QTimer>

namespace Ui {
class setup;
}

class setup : public QWidget
{
    Q_OBJECT

public:
    explicit setup(QWidget *parent = 0);
    ~setup();
    void setEngPtr(setEng *p) { m_eng = p; }

private slots:
    void on_backBt_clicked();
    void on_nav1_clicked();
    void on_nav2_clicked();
    void on_nav3_clicked();
    void on_nav4_clicked();
    void on_nav5_clicked();
    void on_saveBt_clicked();
    void on_speedChanged(int val);
    void on_resetBt_clicked();
    void on_ckDoor_clicked();
    void on_ckWarehouse_clicked();
    void on_ckParking_clicked();
    void on_ckWall_clicked();
    void onToggleCruise(bool checked);
    void onCruiseTick();

private:
    Ui::setup *ui;
    setEng *m_eng;

    struct Preset {
        int lr;
        int ud;
        bool enabled;
    };
    Preset m_presets[4];

    int  m_cruiseIndex;
    int  m_cruiseCount;
    int  m_cruiseSpeed;
    QTimer *m_cruiseTimer;
    bool m_cruiseRunning;

    void startCruise();
    void stopCruise();
    void selectCategory(int index);
    void clearSelection();
};

#endif // SETUP_H
