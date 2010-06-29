load(qttest_p4)
SOURCES += tst_qxmlstream.cpp

QT = core xml network


wince*|symbian: {
   addFiles.sources = data XML-Test-Suite
   addFiles.path = .
   DEPLOYMENT += addFiles
   DEFINES += SRCDIR=\\\"\\\"
} else {
    DEFINES += SRCDIR=\\\"$$PWD/\\\"
}
