
TEMPLATE = subdirs
CONFIG += ordered

common.file = ../../Core/Core.pro

pulseaudio.file = pulseaudio/pulseaudio.pro
pulseaudio.depends = common
SUBDIRS += pulseaudio

