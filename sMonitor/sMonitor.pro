#-------------------------------------------------
#
# Project created by QtCreator 2026-06-23T19:23:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sMonitor
TEMPLATE = app


SOURCES += main.cpp        setup.cpp\
        menu.cpp \
    fops.cpp \
    unusual.cpp \
    steng.cpp

HEADERS  += menu.h        setup.h \
    fops.h \
    unusual.h \
    steng.h

FORMS    += menu.ui        setup.ui \
    fops.ui \
    unusual.ui

RESOURCES += \
    images.qrc
