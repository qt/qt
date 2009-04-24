TEMPLATE = app
CONFIG += qt
HEADERS = ../environment.h uninstallimpl.h
SOURCES = uninstaller.cpp ../environment.cpp uninstallimpl.cpp
INTERFACES = uninstall.ui
TARGET  = quninstall
DESTDIR = ../../../../dist/win/bin
