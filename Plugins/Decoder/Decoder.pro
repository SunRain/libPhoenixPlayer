
TEMPLATE = subdirs
CONFIG += ordered

common.file = ../../libPhoenixPlayer.pro

ffmpeg.file = ffmpeg/ffmpeg.pro
ffmpeg.depends = common
SUBDIRS += ffmpeg

