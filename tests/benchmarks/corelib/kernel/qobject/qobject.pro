load(qttest_p4)
TEMPLATE = app
TARGET = tst_qobject
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += object.h
SOURCES += main.cpp object.cpp
