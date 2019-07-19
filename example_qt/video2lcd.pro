#-------------------------------------------------
#
# Project created by QtCreator 2019-07-17T15:56:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = video2lcd
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    color.cpp \
    libv4l2.cpp

HEADERS  += widget.h \
    color.h \
    libv4l2.h

FORMS    += widget.ui
