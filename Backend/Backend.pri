#TEMPLATE = lib
#CONFIG += plugin

#HEADERS += ../core/include/IPlayBackend.h \
#    ../backend/GStreamer/Util.h
##SOURCES += ../../common.cpp ../abstractWaveformBuilder.cpp ../../waveformPeaks.cpp

#unix:DESTDIR = ../../../plugins
#win32:DESTDIR = ../../../Plugins

#OBJECTS_DIR = $$TMP_DIR
#MOC_DIR = $$TMP_DIR

#INCLUDEPATH += .. ../.. ../../interfaces
#DEPENDPATH += ../..

include(GStreamer/GStreamer.pri)
