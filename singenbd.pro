#-------------------------------------------------
#
# Project created by QtCreator 2015-12-15T15:42:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = singenbd
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    counter.cpp \
    waveform.cpp

HEADERS  += mainwindow.h \
    counter.h \
    waveform.h

FORMS    += mainwindow.ui
