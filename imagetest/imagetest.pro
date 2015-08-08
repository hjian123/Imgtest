#-------------------------------------------------
#
# Project created by QtCreator 2015-08-01T10:30:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = imagetest
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    iputool.cpp \
    v4l2cap.cpp

HEADERS  += mainwindow.h \
    iputool.h \
    v4l2cap.h

FORMS    += mainwindow.ui

#INCLUDEPATH += ./
#LIBS += -L/home/hj/Qt-projects/imagetest/libs -lcapyuv2rgb
