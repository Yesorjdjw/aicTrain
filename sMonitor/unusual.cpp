#include "unusual.h"
#include "ui_unusual.h"
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLabel>

unusual::unusual(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::unusual),
    m_currentIndex(0)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(0, 0, 1024, 600);
    initTable();
    loadDefaultData();
    selectCategory(0);
}

unusual::~unusual()
{
    delete ui;
}

void unusual::on_backBt_clicked()
{
    this->close();
}

void unusual::setData(const QList<CategoryData> &categories)
{
    m_categories = categories;
    QLabel *badges[] = {ui->badge1, ui->badge2, ui->badge3, ui->badge4, ui->badge5};
    int total = 0;
    int maxItems = qMin(m_categories.size(), 5);
    for (int i = 0; i < maxItems; i++) {
        badges[i]->setText(QString::number(m_categories[i].count));
        badges[i]->show();
        total += m_categories[i].count;
    }
    for (int i = maxItems; i < 5; i++) {
        badges[i]->hide();
    }
    ui->badgeTotal->setText(QString::number(total));
    selectCategory(0);
}

void unusual::initTable()
{
    ui->eventTable->setColumnWidth(0, 130);
    ui->eventTable->setColumnWidth(1, 160);
    ui->eventTable->setColumnWidth(2, 70);
    ui->eventTable->setColumnWidth(3, 180);
    ui->eventTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
}

void unusual::loadDefaultData()
{
    m_categories.clear();
}

void unusual::selectCategory(int index)
{
    if (m_categories.isEmpty() || index < 0 || index >= m_categories.size()) return;
    m_currentIndex = index;
    const CategoryData &cat = m_categories[index];

    clearSelection();
    const QString selStyle =
        "background: rgba(0,180,255,0.2); border: none; border-radius: 0px;"
        "text-align: left; font-size: 13px; font-weight: 600;"
        "color: #333333; padding-left: 52px;";
    QPushButton *btn = findChild<QPushButton*>(QString("catBtn%1").arg(index + 1));
    if (btn) btn->setStyleSheet(selStyle);

    QLabel *icon = findChild<QLabel*>(QString("catIcon%1").arg(index + 1));
    if (icon) icon->raise();

    ui->rightTitleText->setText(cat.name);
    ui->countText->setText(QString("共 %1 条").arg(cat.count));
    ui->rightIcon->setPixmap(QPixmap(cat.iconPath));
    setupTable(cat.events);
}

void unusual::setupTable(const QList<EventData> &events)
{
    ui->eventTable->clearContents();
    ui->eventTable->setRowCount(events.size());

    for (int i = 0; i < events.size(); i++) {
        ui->eventTable->setRowHeight(i, 54);
        const EventData &e = events[i];
        QStringList cols;
        cols << e.screenshot << e.type << e.level << e.time << e.status;
        for (int j = 0; j < 5; j++) {
            QTableWidgetItem *item = new QTableWidgetItem(cols[j]);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            ui->eventTable->setItem(i, j, item);
        }

        // 等级列颜色
        QTableWidgetItem *levelItem = ui->eventTable->item(i, 2);
        if (levelItem) {
            if (e.level == QString::fromUtf8("紧急"))
                levelItem->setForeground(QColor("#ff3b30"));
            else if (e.level == QString::fromUtf8("警告"))
                levelItem->setForeground(QColor("#ff9500"));
            else if (e.level == QString::fromUtf8("通知"))
                levelItem->setForeground(QColor("#007aff"));
        }

        // 状态列颜色
        QTableWidgetItem *statusItem = ui->eventTable->item(i, 4);
        if (statusItem) {
            if (e.status == QString::fromUtf8("待处理"))
                statusItem->setForeground(QColor("#ff9500"));
            else if (e.status == QString::fromUtf8("处理中"))
                statusItem->setForeground(QColor("#1a7bbd"));
        }
    }
}

void unusual::clearSelection()
{
    const QString normalStyle =
        "background: transparent; border: none; border-radius: 0px;"
        "text-align: left; font-size: 13px; font-weight: 600;"
        "color: #333333; padding-left: 52px;";
    for (int i = 1; i <= 5; i++) {
        QPushButton *btn = findChild<QPushButton*>(QString("catBtn%1").arg(i));
        if (btn) btn->setStyleSheet(normalStyle);
    }
}

void unusual::on_catBtn1_clicked() { selectCategory(0); }
void unusual::on_catBtn2_clicked() { selectCategory(1); }
void unusual::on_catBtn3_clicked() { selectCategory(2); }
void unusual::on_catBtn4_clicked() { selectCategory(3); }
void unusual::on_catBtn5_clicked() { selectCategory(4); }
