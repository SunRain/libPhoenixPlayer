include (../libPhoenixPlayer.pri)
include (../include.pri)

PLUGINS_PREFIX = ../../../target/plugins

contains (CONFIG, WITH_QML_LIB) {
    include (../libPhoenixPlayer/QtQuick/qml.pri)
    QT += quick
}

win32 {
    COPY = copy /y
    MKDIR = mkdir
} else {
    COPY = cp
    MKDIR = mkdir -p
}
