
TEMPLATE = subdirs
CONFIG += ordered

common.file = ../../libPhoenixPlayer.pro

baidu.file = Baidu/Baidu.pro
baidu.depends = common
SUBDIRS += baidu

lastfm.file = LastFM/LastFM.pro
lastfm.depends = common
SUBDIRS += lastfm
