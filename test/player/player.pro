#-------------------------------------------------
#
# Project created by QtCreator 2015-01-08T19:43:01
#
#-------------------------------------------------

QT       += core qml quick
#QT       -= gui

TARGET = testplayer
CONFIG   += console
#CONFIG   -= app_bundle
#CONFIG += c++11

#QMAKE_CXXFLAGS += -std=c++11

#gcc:CXXFLAGS += -std=c++0x

CONFIG += c++11

TEMPLATE = app


SOURCES += main.cpp \

unix {
    INCLUDEPATH += \
#        $$PWD/../core/include \
#        $$PWD/../backend/GStreamerBackend \
#        /usr/include/gstreamer-0.10 \
#        /usr/include/glib-2.0 \
#        /usr/include/libxml2
}

#unix: LIBS += -L$$OUT_PWD/../PlayBackend -lplugin_gstreamer

#SOURCES += \
#    main.cpp \

RESOURCES += \
    qml.qrc


# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)


include(../../Core/CoreHeaders.pri)
include (../../Plugins/QML/QmlPluginHeaders.pri)


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../Core/release/ -lPhoenixPlayer
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../Core/debug/ -lPhoenixPlayer
else:unix: LIBS += -L$$OUT_PWD/../../Core/ -lPhoenixPlayer

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../Plugins/QML/release/ -lqmlplugins
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../Plugins/QML/debug/ -lqmlplugins
else:unix: LIBS += -L$$OUT_PWD/../../Plugins/QML/ -lqmlplugins

