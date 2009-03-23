TEMPLATE = app
TARGET 	 = unpackage
mac:TARGET = qt-mac-commercial-3.0.0
mac:RC_FILE = unpackage.icns
HEADERS = unpackdlgimpl.h licensedlgimpl.h
SOURCES = main.cpp unpackdlgimpl.cpp licensedlgimpl.cpp
INTERFACES += unpackdlg.ui licensedlg.ui
INCLUDEPATH += ../archive ../keygen
CONFIG += qt
unix:LIBS += -L$$QT_BUILD_TREE/util/install/archive -larq

