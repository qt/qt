load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
macx:CONFIG -= app_bundle

SOURCES += tst_parserstress.cpp

symbian: {
    importFiles.sources = ..\\..\\qscriptjstestsuite\\tests
    importFiles.path = .
    DEPLOYMENT = importFiles
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

CONFIG += parallel_test

