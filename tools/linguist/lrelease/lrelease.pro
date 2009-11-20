TEMPLATE        = app
TARGET          = lrelease
DESTDIR         = ../../../bin

DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII
SOURCES += main.cpp

include(../../../src/tools/bootstrap/bootstrap.pri)
include(../shared/formats.pri)
include(../shared/proparser.pri)

win32-msvc*:LIBS += advapi32.lib   # for qsettings_win.cpp

target.path=$$[QT_INSTALL_BINS]
INSTALLS        += target
