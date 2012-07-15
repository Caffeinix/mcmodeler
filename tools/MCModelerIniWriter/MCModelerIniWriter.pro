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
    application.h \
    ../../src/block_geometry.h \
    ../../src/enumeration_impl.h \
    ../../src/enumeration.h

INCLUDEPATH += ../../third_party/qjson/include

macx {
    QMAKE_LFLAGS += -F ../../third_party/qjson/lib
    LIBS += -framework qjson
    QMAKE_POST_LINK += echo "Running install_name_tool..."; \
                       install_name_tool -id @loader_path/../../third_party/qjson/lib/qjson.framework/Versions/0/qjson \
                                             ../../third_party/qjson/lib/qjson.framework/Versions/0/qjson; \
                       install_name_tool -change qjson.framework/Versions/0/qjson \
                                                 @loader_path/../../third_party/qjson/lib/qjson.framework/Versions/0/qjson \
                                                 $$OUT_PWD/$$TARGET;

    QJsonFramework.files = ../../third_party/qjson/lib/qjson.framework
    QJsonFramework.path = Contents/Frameworks
    QMAKE_BUNDLE_DATA += QJsonFramework
}

win32:LIBS += ../../third_party/qjson/lib/qjson0.dll
