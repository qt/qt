TARGET     = QtMultimedia
QPRO_PWD   = $$PWD
QT         = core
DEFINES   += QT_BUILD_MULTIMEDIA_LIB

win32-msvc*:QMAKE_LIBS += $$QMAKE_LIBS_CORE

unix:QMAKE_PKGCONFIG_REQUIRES = QtCore

include(../qbase.pri)
include(audio/audio.pri)
