#ifndef UNUSUAL_H
#define UNUSUAL_H

#include <QWidget>
#include <QLabel>
#include <QStringList>

namespace Ui {
class unusual;
}

struct EventData {
    QString screenshot;
    QString type;
    QString level;
    QString time;
    QString status;
};

struct CategoryData {
    QString name;
    QString location;
    QString iconPath;
    int count;
    QList<EventData> events;
};

class unusual : public QWidget
{
    Q_OBJECT

public:
    explicit unusual(QWidget *parent = 0);
    ~unusual();

    void setData(const QList<CategoryData> &categories);

private slots:
    void on_backBt_clicked();
    void on_catBtn1_clicked();
    void on_catBtn2_clicked();
    void on_catBtn3_clicked();
    void on_catBtn4_clicked();
    void on_catBtn5_clicked();

private:
    Ui::unusual *ui;
    QList<CategoryData> m_categories;
    int m_currentIndex;

    void selectCategory(int index);
    void setupTable(const QList<EventData> &events);
    void clearSelection();
    void initTable();
    void loadDefaultData();
};
#endif
