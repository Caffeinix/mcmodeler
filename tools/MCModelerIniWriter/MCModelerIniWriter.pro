#-------------------------------------------------
#
# Project created by QtCreator 2012-06-20T00:34:20
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = MCModelerIniWriter
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cc \
    application.cc

HEADERS += \
    application.h

INCLUDEPATH += ../../third_party/qjson/include
LIBS += ../../third_party/qjson/lib/qjson0.dll
