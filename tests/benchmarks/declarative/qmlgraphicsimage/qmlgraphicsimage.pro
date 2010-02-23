load(qttest_p4)
TEMPLATE = app
TARGET = tst_qmlgraphicsimage
QT += declarative
macx:CONFIG -= app_bundle
CONFIG += release

SOURCES += tst_qmlgraphicsimage.cpp

DEFINES += SRCDIR=\\\"$$PWD\\\"

