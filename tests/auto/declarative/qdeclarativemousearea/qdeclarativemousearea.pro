load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative gui network
macx:CONFIG -= app_bundle

HEADERS += ../shared/testhttpserver.h
SOURCES += tst_qdeclarativemousearea.cpp ../shared/testhttpserver.cpp

symbian: {
    importFiles.sources = data
    importFiles.path = .
    DEPLOYMENT = importFiles
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

CONFIG += parallel_test

