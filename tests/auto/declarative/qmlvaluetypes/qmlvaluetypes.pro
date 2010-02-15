load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
macx:CONFIG -= app_bundle

HEADERS += testtypes.h

SOURCES += tst_qmlvaluetypes.cpp \
           testtypes.cpp

DEFINES += SRCDIR=\\\"$$PWD\\\"
