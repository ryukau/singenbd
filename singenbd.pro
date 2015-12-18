#-------------------------------------------------
#
# Project created by QtCreator 2015-12-15T15:42:16
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = singenbd
TEMPLATE = app
CONFIG += c++14


SOURCES += main.cpp\
        mainwindow.cpp \
    counter.cpp \
    waveform.cpp \
    soundplayer.cpp \
    utils.cpp \
    FMTOComplex.cpp \
    TaikoOperator.cpp

HEADERS  += mainwindow.h \
    counter.h \
    waveform.h \
    soundplayer.h \
    utils.h \
    FMTOComplex.h \
    TaikoOperator.h

FORMS    += mainwindow.ui
