TEMPLATE = lib
CONFIG += plugin

TARGET = TagParserPro

QT += gui

#Enable c++11
CONFIG += c++11

#TODO 暂时链接库文件
include(../../../Common/Common.pri)

include(../../../Common/sdk.pri)
include (TagParserPro.pri)

INCLUDEPATH += \
        $$PWD

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../Common/release/ -lCommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../Common/debug/ -lCommon
else:unix: LIBS += -L$$OUT_PWD/../../../Common/ -lCommon

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += taglib
}

DESTDIR = ../../../plugins

win32 {
        INCLUDEPATH += $(TAGLIB_DIR)/include
        LIBS += -L$(TAGLIB_DIR)/lib -ltag
        DEFINES += TAGLIB_STATIC
}

