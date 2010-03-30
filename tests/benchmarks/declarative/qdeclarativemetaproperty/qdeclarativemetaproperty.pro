load(qttest_p4)
TEMPLATE = app
TARGET = tst_qdeclarativemetaproperty
QT += declarative
macx:CONFIG -= app_bundle

SOURCES += tst_qdeclarativemetaproperty.cpp 

# Define SRCDIR equal to test's source directory
DEFINES += SRCDIR=\\\"$$PWD\\\"
