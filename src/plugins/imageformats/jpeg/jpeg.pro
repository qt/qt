TARGET  = qjpeg
include(../../qpluginbase.pri)

QTDIR_build:REQUIRES = "!contains(QT_CONFIG, no-jpeg)"

wince*: {
    DEFINES += NO_GETENV
    contains(CE_ARCH,x86):CONFIG -= stl exceptions
    contains(CE_ARCH,x86):CONFIG += exceptions_off
}

#Disable warnings in 3rdparty code due to unused arguments
symbian: {
        QMAKE_CXXFLAGS.CW += -W nounusedarg
        TARGET.UID3=0x2001E61B
} else:contains(QMAKE_CC, gcc): {
    QMAKE_CFLAGS_WARN_ON += -Wno-unused-parameter -Wno-main
}

include(../../../gui/image/qjpeghandler.pri)
SOURCES += main.cpp

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/imageformats
target.path += $$[QT_INSTALL_PLUGINS]/imageformats
INSTALLS += target
