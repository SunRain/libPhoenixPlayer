
TEMPLATE = subdirs
CONFIG += ordered


common.file = ../../Core/Core.pro

sqlite3.file = SQLite3/SQLite3.pro
sqlite3.depends = common
# musiclibrarycore
SUBDIRS += sqlite3

tagParserPro.file = TagParserPro/TagParserPro.pro
tagParserPro.depends = common
#musiclibrarycore
SUBDIRS += tagParserPro

#tagParserID3v1.file = Plugins/TagParserID3v1/TagParserID3v1.pro
#tagParserID3v1.depends = common musiclibrarycore
#SUBDIRS += tagParserID3v1

#tagParserID3v2.file = Plugins/TagParserID3v2/TagParserID3v2.pro
#tagParserID3v2.depends = common musiclibrarycore
#SUBDIRS += tagParserID3v2

#SUBDIRS += \
#    Core/MusicLibraryCore.pro  \
#    Plugins



