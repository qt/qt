TARGET = qtga
include(../../qpluginbase.pri)

QTDIR_build:REQUIRES = "!contains(QT_CONFIG, no-tga)"

HEADERS += qtgahandler.h \
    qtgafile.h
SOURCES += main.cpp \
    qtgahandler.cpp \
    qtgafile.cpp

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/imageformats
target.path += $$[QT_INSTALL_PLUGINS]/imageformats
INSTALLS += target

symbian: TARGET.UID3=0x20031E99
