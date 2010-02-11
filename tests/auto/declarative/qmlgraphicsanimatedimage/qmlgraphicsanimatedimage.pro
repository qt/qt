load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative network
HEADERS += ../shared/testhttpserver.h
SOURCES += tst_qmlgraphicsanimatedimage.cpp ../shared/testhttpserver.cpp
macx:CONFIG -= app_bundle

DEFINES += SRCDIR=\\\"$$PWD\\\"
