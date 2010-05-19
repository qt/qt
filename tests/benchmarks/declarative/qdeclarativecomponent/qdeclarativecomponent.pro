load(qttest_p4)
TEMPLATE = app
TARGET = tst_qdeclarativecomponent
QT += declarative
macx:CONFIG -= app_bundle

SOURCES += tst_qdeclarativecomponent.cpp testtypes.cpp
HEADERS += testtypes.h

# Define SRCDIR equal to test's source directory
symbian: {
    DEFINES += SRCDIR=\".\"
    importFiles.sources = data
    importFiles.path =
    DEPLOYMENT = importFiles
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}
