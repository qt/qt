load(qttest_p4)
TEMPLATE = app
TARGET = tst_qmlgraphicsimage
QT += declarative network
macx:CONFIG -= app_bundle
CONFIG += release

INCLUDEPATH += ../../../auto/declarative/shared
SOURCES += tst_qmlgraphicsimage.cpp ../../../auto/declarative/shared/testhttpserver.cpp
HEADERS += ../../../auto/declarative/shared/testhttpserver.h


DEFINES += SRCDIR=\\\"$$PWD\\\"

