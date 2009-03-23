############################################################
# Project file for autotest for file mediaobject.h
############################################################

load(qttest_p4)

contains(QT_CONFIG, phonon):QT += phonon

wince*{
    SOURCES += ../mediaobject/tst_mediaobject.cpp
    HEADERS += ../mediaobject/qtesthelper.h
    RESOURCES += ../mediaobject/mediaobject.qrc
    DEPLOYMENT_PLUGIN += phonon_ds9
    DEFINES += tst_MediaObject=tst_MediaObject_ds9
} else {
    SOURCES += dummy.cpp
}

