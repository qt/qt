load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative script
macx:CONFIG -= app_bundle

SOURCES += tst_qdeclarativescriptdebugging.cpp
INCLUDEPATH += ../shared

# QMAKE_CXXFLAGS = -fprofile-arcs -ftest-coverage
# LIBS += -lgcov

symbian: {
    importFiles.sources = data
    importFiles.path = .
    DEPLOYMENT = importFiles
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

CONFIG += parallel_test

