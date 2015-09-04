
TEMPLATE = subdirs
CONFIG += ordered

common.file = ../../Core/Core.pro

ffmpeg.file = ffmpeg/ffmpeg.pro
ffmpeg.depends = common
SUBDIRS += ffmpeg

