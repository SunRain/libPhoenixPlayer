
TEMPLATE = lib
CONFIG += plugin

TARGET = FakeBackend

include(../../Core/backendInterface.pri)
include(../../../Common/Common.pri)
include(FakeBackend.pri)

INCLUDEPATH += \
        $$PWD

unix {
#    CONFIG += link_pkgconfig
#    PKGCONFIG += gstreamer-0.10

#    INCLUDEPATH += \
#        /usr/include/gstreamer-0.10 \
#        /usr/include/glib-2.0 \
#        /usr/include/libxml2
}

DESTDIR = ../../../plugins

#PlayBackend
win32 {
#	INCLUDEPATH +=  $(OSSBUILD_GSTREAMER_SDK_DIR)/include \
#	                $(OSSBUILD_GSTREAMER_SDK_DIR)/include/gstreamer-0.10 \
#	                $(OSSBUILD_GSTREAMER_SDK_DIR)/include/glib-2.0 \
#	                $(OSSBUILD_GSTREAMER_SDK_DIR)/include/libxml2

#	LIBS +=         -L$(OSSBUILD_GSTREAMER_SDK_DIR)/lib \
#	                -lgstreamer-0.10 \
#	                -lglib-2.0 \
#	                -liconv \
#	                -lxml2 \
#	                -lgobject-2.0

#	DEFINES += LIBXML_STATIC
}


#gstreamer-tagreader {
#	unix:PKGCONFIG += gstreamer-pbutils-0.10
#	DEFINES += _N_GSTREAMER_TAGREADER_PLUGIN_
#} else {
#	HEADERS -= tagReaderGstreamer.h
#	SOURCES -= tagReaderGstreamer.cpp
#}


