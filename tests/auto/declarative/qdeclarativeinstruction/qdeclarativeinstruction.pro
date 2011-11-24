load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative script
SOURCES += tst_qdeclarativeinstruction.cpp
macx:CONFIG -= app_bundle

wince*: {
    DEFINES += SRCDIR=\\\".\\\"
} else:!symbian: {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

CONFIG += parallel_test

