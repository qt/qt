############################################################
# Project file for autotest for file qsvggenerator.h
############################################################

load(qttest_p4)
QT += svg xml

SOURCES += tst_qsvggenerator.cpp

wince*: {
    addFiles.sources = referenceSvgs
    addFiles.path = .
    DEPLOYMENT += addFiles
    DEFINES += SRCDIR=\\\"\\\"
} else {
    DEFINES += SRCDIR=\\\"$$PWD/\\\"
}
