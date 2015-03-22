TEMPLATE = lib
CONFIG += plugin

QT += network

TARGET = LastFmMetaDataLookup

#Enable c++11
CONFIG += c++11

#TODO 暂时链接库文件
include(../../../Common/Common.pri)
include(../../../Common/sdk.pri)
include (../../Core/MetadataLookupCore.pri)

include (LastFM.pri)

INCLUDEPATH += \
        $$PWD

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../Common/release/ -lCommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../Common/debug/ -lCommon
else:unix: LIBS += -L$$OUT_PWD/../../../Common/ -lCommon

DESTDIR = ../../../plugins


