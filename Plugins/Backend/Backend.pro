
TEMPLATE = subdirs
CONFIG += ordered

common.file = ../../Core/Core.pro

!win32 {
gstreamer.file = GStreamerBackend/GStreamerBackend.pro
gstreamer.depends = common
SUBDIRS += gstreamer
}
fake.file = FakeBackend/FakeBackend.pro
gstreamer.depends = common
SUBDIRS += fake


