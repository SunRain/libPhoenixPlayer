
TEMPLATE = subdirs
CONFIG += ordered

common.file = ../../libPhoenixPlayer.pro

pulseaudio.file = pulseaudio/pulseaudio.pro
pulseaudio.depends = common
SUBDIRS += pulseaudio

