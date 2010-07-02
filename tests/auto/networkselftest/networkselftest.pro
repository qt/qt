load(qttest_p4)

SOURCES += tst_networkselftest.cpp
QT = core network

wince*: {
    addFiles.sources = rfc3252.txt
    addFiles.path = .
    DEPLOYMENT = addFiles
    DEFINES += SRCDIR=\\\"\\\"
} else:symbian {
    addFiles.sources = rfc3252.txt
    addFiles.path = .
    DEPLOYMENT = addFiles
} else:vxworks*: {
    DEFINES += SRCDIR=\\\"\\\"
} else {
    DEFINES += SRCDIR=\\\"$$PWD/\\\"
}

