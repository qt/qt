load(qttest_p4)
SOURCES += tst_bic.cpp qbic.cpp
QT = core

wince*:{
    DEFINES += SRCDIR=\\\"\\\"
} else {
    DEFINES += SRCDIR=\\\"$$PWD/\\\"
}

