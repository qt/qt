load(qttest_p4)
contains(QT_CONFIG,declarative): QT += network declarative
macx:CONFIG -= app_bundle

HEADERS += ../shared/debugutil_p.h
SOURCES += tst_qdeclarativedebugclient.cpp \
           ../shared/debugutil.cpp

CONFIG += declarative_debug
