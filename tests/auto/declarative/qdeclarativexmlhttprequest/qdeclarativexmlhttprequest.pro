load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative network
macx:CONFIG -= app_bundle

INCLUDEPATH += ../shared/
HEADERS += ../shared/testhttpserver.h

SOURCES += tst_qdeclarativexmlhttprequest.cpp \
           ../shared/testhttpserver.cpp

# Define SRCDIR equal to test's source directory
symbian: {
    DEFINES += SRCDIR=\".\"
    importFiles.sources = data
    importFiles.path = 
    DEPLOYMENT = importFiles
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

CONFIG += parallel_test

