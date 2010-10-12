############################################################
# Project file for autotest for file qsvggenerator.h
############################################################

load(qttest_p4)
QT += svg xml

SOURCES += tst_qsvggenerator.cpp

wince*|symbian {
    addFiles.files = referenceSvgs
    addFiles.path = .
    DEPLOYMENT += addFiles
}

wince* {
    DEFINES += SRCDIR=\\\"\\\"
} else:!symbian {
    DEFINES += SRCDIR=\\\"$$PWD/\\\"
}
