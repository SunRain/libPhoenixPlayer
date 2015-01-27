
TEMPLATE = subdirs
CONFIG += ordered

common.file = ../Common.pro
#Plugins.depends = common

#SUBDIRS += \
###    Core \
#    Plugins

!win32 {
gstreamer.file = Plugins/GStreamerBackend/GStreamerBackend.pro
gstreamer.depends = common
SUBDIRS += gstreamer
}
fake.file = Plugins/FakeBackend/FakeBackend.pro
gstreamer.depends = common
SUBDIRS += fake


