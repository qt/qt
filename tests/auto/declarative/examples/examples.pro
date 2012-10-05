load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
macx:CONFIG -= app_bundle

SOURCES += tst_examples.cpp 

include(../../../../tools/qml/qml.pri)

include(../symbianlibs.pri)

wince*|symbian: {
    importFiles.files = data
    importFiles.path = .
    DEPLOYMENT += importFiles
    wince*: DEFINES += SRCDIR=\\\".\\\"
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

CONFIG += parallel_test
