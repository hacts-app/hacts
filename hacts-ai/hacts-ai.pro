#-------------------------------------------------
#
# Project created by QtCreator 2018-05-14T17:43:02
#
#-------------------------------------------------

TARGET = hacts-ai
TEMPLATE = app

QT -= core gui


CONFIG += c++17
QMAKE_CXXFLAGS += -std=gnu++17 -ffast-math

DESTDIR = $$_PRO_FILE_PWD_/../out

INCLUDEPATH += $$_PRO_FILE_PWD_/../lib

SOURCES += \
    main.cpp \
    inputhandler.cpp \
    processCommand.cpp \
    car.cpp \
    rectangle.cpp \
    mapElements.cpp \
    functions.cpp \
    gps.cpp

HEADERS += \
    inputhandler.h \
    processCommand.h \
    car.h \
    rectangle.h \
    mapElements.h \
    functions.h \
    gps.h

CONFIG += console
