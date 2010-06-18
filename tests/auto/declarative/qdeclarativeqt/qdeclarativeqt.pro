load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
SOURCES += tst_qdeclarativeqt.cpp
macx:CONFIG -= app_bundle

symbian: {
    importFiles.sources = data
    importFiles.path = .
    DEPLOYMENT = importFiles
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

# QMAKE_CXXFLAGS = -fprofile-arcs -ftest-coverage
# LIBS += -lgcov

CONFIG += parallel_test

