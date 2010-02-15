load(qttest_p4)
TARGET = tst_qstringlist
CONFIG -= debug
CONFIG += release
QT -= gui
SOURCES += main.cpp

symbian: LIBS += -llibpthread
