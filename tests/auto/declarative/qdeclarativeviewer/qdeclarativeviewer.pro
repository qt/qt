load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative gui
macx:CONFIG -= app_bundle

include(../../../../tools/qml/qml.pri)

SOURCES += tst_qdeclarativeviewer.cpp

symbian: {
    importFiles.sources = data
    importFiles.path = .
    DEPLOYMENT = importFiles
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

CONFIG += parallel_test
