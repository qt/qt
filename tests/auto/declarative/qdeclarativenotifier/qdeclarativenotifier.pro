load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
SOURCES += tst_qdeclarativenotifier.cpp
macx:CONFIG -= app_bundle

wince*: {
    DEFINES += SRCDIR=\\\".\\\"
} else:!symbian: {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

CONFIG += parallel_test
