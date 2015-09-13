include (../../plugins.pri)

TEMPLATE = lib
CONFIG += plugin \
          link_pkgconfig

TARGET = $$PLUGINS_PREFIX/ffmpeg_decoder

include(ffmpeg.pri)

PKGCONFIG += libavcodec libavformat libavutil

DEFINES += __STDC_CONSTANT_MACROS

INCLUDEPATH += \
        $$PWD

#TODO For Ubuntu touch, need test
load(ubuntu-click)

isEmpty (LIB_DIR){
    LIB_DIR = /opt/PhoenixPlayer
}

QMAKE_LIBDIR += ../../../target/lib
LIBS += -lPhoenixPlayer

json.files = $${OTHER_FILES}

!isEmpty(UBUNTU_MANIFEST_FILE){
    DEFINES += UBUNTU_TOUCH
    target.path = $${UBUNTU_CLICK_PLUGIN_PATH}/lib
    json.path = $${UBUNTU_CLICK_PLUGIN_PATH}/lib
} else {
    target.path = $$LIB_DIR/plugins
    json.path = $$LIB_DIR/plugins
}

INSTALLS += target json
