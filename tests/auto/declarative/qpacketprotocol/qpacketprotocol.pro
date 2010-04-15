load(qttest_p4)
contains(QT_CONFIG,declarative): QT += network declarative
macx:CONFIG -= app_bundle

HEADERS += ../shared/debugutil_p.h
SOURCES += tst_qpacketprotocol.cpp \
           ../shared/debugutil.cpp

CONFIG += parallel_test
