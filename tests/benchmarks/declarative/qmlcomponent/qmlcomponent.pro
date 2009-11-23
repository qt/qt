load(qttest_p4)
TEMPLATE = app
TARGET = tst_qmlcomponent
QT += declarative
macx:CONFIG -= app_bundle

SOURCES += tst_qmlcomponent.cpp testtypes.cpp
HEADERS += testtypes.h

