load(qttest_p4)
QT = core
HEADERS += 
SOURCES += tst_qtextboundaryfinder.cpp 
integrity {
    DEFINES += SRCDIR=\"/\"
} else:!symbian {
    DEFINES += SRCDIR=\\\"$$PWD/\\\"
}

wince*|symbian:{
   addFiles.files = data
   addFiles.path = .
   DEPLOYMENT += addFiles
}
CONFIG += parallel_test
