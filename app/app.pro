QT = core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FasterThanSight
TEMPLATE = app


SOURCES += \
    src\main.cpp\
    src\cmainwindow.cpp

HEADERS  += src\cmainwindow.h

FORMS    += src\cmainwindow.ui
