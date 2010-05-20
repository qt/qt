load(qttest_p4)
TEMPLATE = app
TARGET = tst_qdeclarativecomponent
QT += declarative
macx:CONFIG -= app_bundle

SOURCES += tst_qdeclarativecomponent.cpp testtypes.cpp
HEADERS += testtypes.h

symbian {
    data.sources = data
    data.path = .
    DEPLOYMENT += data
} else {
    # Define SRCDIR equal to test's source directory
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}
