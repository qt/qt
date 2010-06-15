load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
contains(QT_CONFIG,webkit): QT += webkit
macx:CONFIG -= app_bundle

SOURCES += tst_qdeclarativewebview.cpp

symbian: {
    importFiles.sources = data
    importFiles.path = .
    DEPLOYMENT = importFiles
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

CONFIG += parallel_test
