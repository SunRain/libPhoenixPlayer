
INCLUDEPATH += $$PWD/CuteLogger/include
DEPENDPATH += $$PWD/CuteLogger/include

HEADERS += \
    $$PWD/CuteLogger/include/AbstractAppender.h \
    $$PWD/CuteLogger/include/AbstractStringAppender.h \
    $$PWD/CuteLogger/include/ConsoleAppender.h \
    $$PWD/CuteLogger/include/CuteLogger_global.h \
    $$PWD/CuteLogger/include/FileAppender.h \
    $$PWD/CuteLogger/include/Logger.h \
    $$PWD/CuteLogger/include/RollingFileAppender.h

win32 {
    HEADERS += $$PWD/CuteLogger/include/OutputDebugAppender.h
}
