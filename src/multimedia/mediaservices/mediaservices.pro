TARGET = QtMediaServices
QPRO_PWD = $$PWD
QT = core gui multimedia

DEFINES += QT_BUILD_MEDIASERVICES_LIB QT_NO_USING_NAMESPACE

unix:QMAKE_PKGCONFIG_REQUIRES = QtCore QtGui QtMultimedia

include(../../qbase.pri)

include(base/base.pri)
include(playback/playback.pri)
include(effects/effects.pri)

symbian: {
    TARGET.UID3 = 0x2001E631
    contains(CONFIG, def_files) {
        DEF_FILE=../../s60installs
    }
}

