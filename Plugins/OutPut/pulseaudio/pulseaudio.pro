include (../../plugins.pri)

TEMPLATE = lib
CONFIG += \
        plugin \
        thread \
        warn_on \
        link_pkgconfig

TARGET = $$PLUGINS_PREFIX/pulseaudio

include(pulseaudio.pri)

PKGCONFIG += libpulse-simple

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

!equals($${_PRO_FILE_PWD_}, $${OUT_PWD}) {
    for(f, OTHER_FILES){
        #TODO need windows basename cmd
        unix:base_name = $$basename(f)
        dist = $${OUT_PWD}/$${PLUGINS_PREFIX}
        dist_file = $${OUT_PWD}/$${PLUGINS_PREFIX}/$${base_name}
        !exists($$dist):system($$MKDIR $$dist)
        !exists($$dist_file):system($$COPY $$f $$dist_file)
    }
}

