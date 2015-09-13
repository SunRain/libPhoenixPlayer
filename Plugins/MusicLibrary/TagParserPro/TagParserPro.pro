include (../../plugins.pri)

TEMPLATE = lib
CONFIG += plugin

TARGET = $$PLUGINS_PREFIX/TagParserPro

QT += gui

include (TagParserPro.pri)

INCLUDEPATH += \
        $$PWD

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += taglib
}

win32 {
        INCLUDEPATH += $(TAGLIB_DIR)/include
        LIBS += -L$(TAGLIB_DIR)/lib -ltag
        DEFINES += TAGLIB_STATIC
}

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
