
TEMPLATE = subdirs
CONFIG += ordered

common.file = ../../libPhoenixPlayer.pro

#!win32 {
#gstreamer.file = GStreamerBackend/GStreamerBackend.pro
#gstreamer.depends = common
#SUBDIRS += gstreamer
#}
#fake.file = FakeBackend/FakeBackend.pro
#gstreamer.depends = common
#SUBDIRS += fake

phoenixbk.file = PhoenixBackend/PhoenixBackend.pro
phoenixbk.depends = common
SUBDIRS += phoenixbk

#QtMultimediaBackend.file = QtMultimediaBackend/QtMultimediaBackend.pro
#QtMultimediaBackend.depends = common
#SUBDIRS += QtMultimediaBackend
