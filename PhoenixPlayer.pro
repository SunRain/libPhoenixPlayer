#-------------------------------------------------
#
# Project created by QtCreator 2015-01-08T19:43:01
#
#-------------------------------------------------

TEMPLATE = subdirs
CONFIG += ordered

common.file = Common/Common.pro
SUBDIRS += common

musiclibrarycore.file = MusicLibrary/Core/MusicLibraryCore.pro
musiclibrarycore.depends = common
SUBDIRS += musiclibrarycore

player.file = Player/Player.pro
player.depends = common musiclibrarycore
SUBDIRS += player

metadatalookupcore.file = MetadataLookup/Core/MetadataLookupCore.pro
metadatalookupcore.depends = common
SUBDIRS += metadatalookupcore

SUBDIRS += \
    Backend \
    MusicLibrary \
    MetadataLookup \
    Plugins \
#    Player \
#    Common \
    test

#gstreamer {
#    gstreamer.file = backend/GStreamer/GStreamer.pro
#    SUBDIRS += gstreamer
#}

#test {
#    test.file = test/test.pro
#    SUBDIRS += test
##    test.depends = backend/GStreamer
#}


