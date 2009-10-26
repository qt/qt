load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
SOURCES += tst_animatedimage.cpp

DEFINES += SRCDIR=\\\"$$PWD\\\"
