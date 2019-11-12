 
UI_DIR=./.build/ui/
MOC_DIR=./.build/moc/
OBJECTS_DIR=./.build/obj
RCC_DIR=./.build/rcc

LIB_VERSION = 1.0

CONFIG += c++11
CONFIG += WITH_QML_LIB
DEFINES += WITH_QML_LIB

QT += network core

QMAKE_CXXFLAGS = -fpermissive

