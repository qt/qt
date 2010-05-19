load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
QT += network
macx:CONFIG -= app_bundle

SOURCES += tst_qdeclarativeimageprovider.cpp

# QMAKE_CXXFLAGS = -fprofile-arcs -ftest-coverage
# LIBS += -lgcov

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

