TEMPLATE = subdirs

!win32 {
gstreamer.file = GStreamerBackend/GStreamerBackend.pro
SUBDIRS += gstreamer
}

#phonon.file = phonon/pluginPhonon.pro
#SUBDIRS += phonon

#vlc.file = vlc/pluginVlc.pro
#SUBDIRS += vlc

#taglib.file = taglib/pluginTaglib.pro
#SUBDIRS += taglib

