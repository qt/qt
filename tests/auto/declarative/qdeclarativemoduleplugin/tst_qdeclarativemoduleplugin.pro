load(qttest_p4)
SOURCES = tst_qdeclarativemoduleplugin.cpp
QT += declarative
CONFIG -= app_bundle

symbian: {
    importFiles.sources = data
    importFiles.path = .
    DEPLOYMENT = importFiles
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}
