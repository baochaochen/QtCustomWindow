#-------------------------------------------------
#
# Project created by QtCreator 2017-08-10T11:53:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CustomWindow
TEMPLATE = app


SOURCES += main.cpp\
        CustomMainWindow.cpp \
    BaseTitleBar.cpp \
    BaseWindow.cpp

HEADERS  += CustomMainWindow.h \
    BaseWindow.h \
    BaseTitleBar.h

FORMS    += CustomMainWindow.ui

RESOURCES += \
    resource.qrc
