TEMPLATE = app

DESTDIR = ./

CONFIG += console
CONFIG -= moc qt app_bundle

SOURCES += main.cpp
#no install rule for child app of test
INSTALLS =


