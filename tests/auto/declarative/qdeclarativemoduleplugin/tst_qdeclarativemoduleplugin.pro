load(qttest_p4)

HEADERS = ../shared/testhttpserver.h
SOURCES = tst_qdeclarativemoduleplugin.cpp \
          ../shared/testhttpserver.cpp
QT += declarative network
CONFIG -= app_bundle

symbian: {
    importFiles.sources = data
    importFiles.path = .
    DEPLOYMENT += importFiles
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}
