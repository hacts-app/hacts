#-------------------------------------------------
#
# Project created by QtCreator 2018-05-14T17:43:02
#
#-------------------------------------------------

TARGET = hacts-ai
TEMPLATE = app

QT -= core gui


CONFIG += c++17
QMAKE_CXXFLAGS += -std=gnu++17

DESTDIR = $$_PRO_FILE_PWD_/../out

SOURCES += \
    main.cpp \
    lib.cpp \
    line.cpp \
    inputhandler.cpp

HEADERS += \
    lib.h \
    line.h \
    inputhandler.h

CONFIG += console
