load(qttest_p4)

INCLUDEPATH += ../

HEADERS +=  
SOURCES += tst_qtextedit.cpp 

wince* {
    DEFINES += SRCDIR=\\\"./\\\"
    addImages.sources = fullWidthSelection/*
    addImages.path = fullWidthSelection
    DEPLOYMENT += addImages
} else {
    DEFINES += SRCDIR=\\\"$$PWD/\\\"
}

contains(QT_CONFIG, qt3support): QT += qt3support
