load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative network
macx:CONFIG -= app_bundle

SOURCES += tst_qdeclarativeengine.cpp 

# Define SRCDIR equal to test's source directory
DEFINES += SRCDIR=\\\"$$PWD\\\"

CONFIG += parallel_test

