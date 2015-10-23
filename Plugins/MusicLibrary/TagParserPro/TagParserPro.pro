include (../../plugins.pri)

TEMPLATE = lib
CONFIG += plugin

TARGET = $$PLUGINS_PREFIX/TagParserPro

QT += gui

include (TagParserPro.pri)
include (../../../ThirdParty/Taglib/Taglib.pri)

INCLUDEPATH += \
        $$PWD

#unix {
#    CONFIG += link_pkgconfig
#    PKGCONFIG += taglib
#}

#win32 {
#        INCLUDEPATH += $(TAGLIB_DIR)/include
#        LIBS += -L$(TAGLIB_DIR)/lib -ltag
#        DEFINES += TAGLIB_STATIC
#}

#TODO For Ubuntu touch, need test
load(ubuntu-click)

isEmpty (LIB_DIR){
    LIB_DIR = /opt/PhoenixPlayer
}

QMAKE_LIBDIR += \
    ../lib \
    ../../../target/lib \
    $${OUT_PWD}/../../../target/lib \
    $${LIB_DIR}/lib

QMAKE_RPATHDIR += \
    ../lib \
    ../../../target/lib \
    $${OUT_PWD}/../../../target/lib \
    $${LIB_DIR}/lib

LIBS += -lPhoenixPlayer -ltag_mod

json.files = $${OTHER_FILES}

!isEmpty(UBUNTU_MANIFEST_FILE){
    DEFINES += UBUNTU_TOUCH
    target.path = $${UBUNTU_CLICK_PLUGIN_PATH}/lib
    json.path = $${UBUNTU_CLICK_PLUGIN_PATH}/lib
    QMAKE_RPATHDIR += \
        $${UBUNTU_CLICK_PLUGIN_PATH}/lib

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

TAGLIB_BUILD_DIR = $$PWD/../../../ThirdParty/Taglib/build
TAGLIB_TARGET_DIR = $$PWD/../../../ThirdParty/Taglib/target
BUILD_TAGLIB = cd $$TAGLIB_BUILD_DIR \
    && cmake ../src -DCMAKE_INSTALL_PREFIX=$$TAGLIB_TARGET_DIR -DCMAKE_RELEASE_TYPE=Release \
    && make \
    && make install
system ($$MKDIR $$TAGLIB_BUILD_DIR)
system ($$MKDIR $$TAGLIB_TARGET_DIR)
system ($$BUILD_TAGLIB)

TAGLIB_FILE = \
    $$PWD/../../../ThirdParty/Taglib/target/lib/*.so \
    $$PWD/../../../ThirdParty/Taglib/target/lib/*.so.*

taglib.files = $${TAGLIB_FILE}
!isEmpty(UBUNTU_MANIFEST_FILE){
    taglib.path = $${UBUNTU_CLICK_PLUGIN_PATH}/lib
} else {
    taglib.path = $${LIB_DIR}/lib
}
INSTALLS += taglib

COPY_LIB = cd $${TAGLIB_TARGET_DIR}/lib \
    && for i in `ls`;do if [ -f $i ];then $$COPY $i $${OUT_PWD}/../../../target/lib;fi;done

system ($$COPY_LIB)
