

unix {
    INCLUDEPATH += \
#        $$PWD/../core/include \
#        $$PWD/../backend/ \
}

#    $$PWD/Gstreamer.h
##SOURCES += ../../common.cpp ../abstractWaveformBuilder.cpp ../../waveformPeaks.cpp

#unix:DESTDIR = ../../../plugins
#win32:DESTDIR = ../../../Plugins

#OBJECTS_DIR = $$TMP_DIR
#MOC_DIR = $$TMP_DIR

#INCLUDEPATH += .. ../.. ../../interfaces
#DEPENDPATH += ../..


OTHER_FILES += \
    $$PWD/tagparserid3v2.json

HEADERS += \
    $$PWD/TagParserID3v2.h

SOURCES += \
    $$PWD/TagParserID3v2.cpp
