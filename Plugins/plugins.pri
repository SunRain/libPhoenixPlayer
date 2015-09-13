include (../libPhoenixPlayer.pri)
include (../include.pri)

PLUGINS_PREFIX = ../../../target/plugins

contains (CONFIG, WITH_QML_LIB) {
    include (QtQuick/qml.pri)
    QT += quick
}
