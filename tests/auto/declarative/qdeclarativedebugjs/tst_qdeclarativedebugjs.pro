load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative network script
macx:CONFIG -= app_bundle

HEADERS += ../shared/debugutil_p.h

SOURCES += tst_qdeclarativedebugjs.cpp \
           ../shared/debugutil.cpp

INCLUDEPATH += ../shared

# QMAKE_CXXFLAGS = -fprofile-arcs -ftest-coverage
# LIBS += -lgcov

symbian: {
    importFiles.files = data
    importFiles.path = .
    DEPLOYMENT += importFiles
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

OTHER_FILES = data/backtrace1.js data/backtrace1.qml

CONFIG += parallel_test
CONFIG+=insignificant_test # QTQAINFRA-428
