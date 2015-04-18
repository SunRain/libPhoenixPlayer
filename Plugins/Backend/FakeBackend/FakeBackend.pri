

HEADERS += \
#    $$PWD/Util.h \
#    $$PWD/GStreamerBackend.h \
    FakeBackend.h

unix {
#    INCLUDEPATH += \
##        $$PWD/../core/include \
##        $$PWD/../backend/ \
#        /usr/include/gstreamer-0.10 \
#        /usr/include/glib-2.0 \
#        /usr/include/libxml2
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
    FakeBackend.cpp

OTHER_FILES += \
    fakebackend.json
