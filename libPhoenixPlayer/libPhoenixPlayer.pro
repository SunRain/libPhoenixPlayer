TEMPLATE = lib
TARGET = ../target/lib/PhoenixPlayer
QT += network core qml

CONFIG += c++11
CONFIG += WITH_QML_LIB

include (../libPhoenixPlayer.pri)

VERSION = $$LIB_VERSION

include (Core/core.pri)
include (QtQuick/qml.pri)

#TODO For Ubuntu touch, need test
load(ubuntu-click)

unix {
    isEmpty(LIB_DIR):LIB_DIR = /opt/PhoenixPlayer/lib
    DESTDIR = .

    !isEmpty(UBUNTU_MANIFEST_FILE){
        DEFINES += UBUNTU_TOUCH
        target.path = $${UBUNTU_CLICK_PLUGIN_PATH}/lib
    } else {
        target.path = $$LIB_DIR
    }
    INSTALLS += target
}

