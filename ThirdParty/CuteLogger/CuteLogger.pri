
INCLUDEPATH += $$PWD/CuteLogger/include
DEPENDPATH += $$PWD/CuteLogger/include

DEFINES += CUTELOGGER_LIBRARY

HEADERS += \
    $$PWD/CuteLogger/include/AbstractAppender.h \
    $$PWD/CuteLogger/include/AbstractStringAppender.h \
    $$PWD/CuteLogger/include/ConsoleAppender.h \
    $$PWD/CuteLogger/include/CuteLogger_global.h \
    $$PWD/CuteLogger/include/FileAppender.h \
    $$PWD/CuteLogger/include/Logger.h \
    $$PWD/CuteLogger/include/RollingFileAppender.h

SOURCES += \
    $$PWD/CuteLogger/src/AbstractAppender.cpp \
    $$PWD/CuteLogger/src/AbstractStringAppender.cpp \
    $$PWD/CuteLogger/src/ConsoleAppender.cpp \
    $$PWD/CuteLogger/src/FileAppender.cpp \
    $$PWD/CuteLogger/src/Logger.cpp \
    $$PWD/CuteLogger/src/RollingFileAppender.cpp


win32 {
    SOURCES += $$PWD/CuteLogger/src/OutputDebugAppender.cpp
    HEADERS += $$PWD/CuteLogger/include/OutputDebugAppender.h
}
