
TEMPLATE = subdirs
CONFIG += ordered


#common.file = ../Common.pro
##Plugins.depends = common

#musiclibrarycore.file = Core/MusicLibraryCore.pro
#musiclibrarycore.depends = common
#SUBDIRS += musiclibrarycore


#tagParserID3v1.file = Plugins/TagParserID3v1/TagParserID3v1.pro
#tagParserID3v1.depends = common musiclibrarycore
#SUBDIRS += tagParserID3v1

#tagParserID3v2.file = Plugins/TagParserID3v2/TagParserID3v2.pro
#tagParserID3v2.depends = common musiclibrarycore
#SUBDIRS += tagParserID3v2

#SUBDIRS += \
#    Core/MusicLibraryCore.pro  \
#    Plugins

baidu.file = Plugin/Baidu/Baidu.pro
baidu.depends = common metadatalookupcore
SUBDIRS += baidu


