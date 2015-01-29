

HEADERS += \
    $$PWD/Util.h \
    $$PWD/GStreamerBackend.h \

unix {
    INCLUDEPATH += \
#        $$PWD/../core/include \
#        $$PWD/../backend/ \
        /usr/include/gstreamer-0.10 \
        /usr/include/glib-2.0 \
        /usr/include/libxml2
}

#    $$PWD/Gstreamer.h
##SOURCES += ../../common.cpp ../abstractWaveformBuilder.cpp ../../waveformPeaks.cpp

#unix:DESTDIR = ../../../plugins
#win32:DESTDIR = ../../../Plugins

#OBJECTS_DIR = $$TMP_DIR
#MOC_DIR = $$TMP_DIR

#INCLUDEPATH += .. ../.. ../../interfaces
#DEPENDPATH += ../..

SOURCES += \
    $$PWD/GStreamerBackend.cpp \

OTHER_FILES += \
    $$PWD/GStreamerBackend.json

DISTFILES += \
    $$PWD/gstreamerbackend.json
