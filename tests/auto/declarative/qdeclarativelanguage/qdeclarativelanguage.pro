load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
QT += script network
macx:CONFIG -= app_bundle

SOURCES += tst_qdeclarativelanguage.cpp \
           testtypes.cpp
HEADERS += testtypes.h

INCLUDEPATH += ../shared/
HEADERS += ../shared/testhttpserver.h
SOURCES += ../shared/testhttpserver.cpp

symbian: {
    importFiles.files = data
    importFiles.path = .
    DEPLOYMENT += importFiles
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

CONFIG += parallel_test
