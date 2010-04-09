TARGET = qtestlitegraphicssystem
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms

SOURCES = main.cpp qplatformintegration_testlite.cpp qwindowsurface_testlite.cpp qtestlitewindow.cpp
HEADERS = qplatformintegration_testlite.h qwindowsurface_testlite.h qtestlitewindow.h

LIBS += -lX11 -lXext


target.path += $$[QT_INSTALL_PLUGINS]/graphicssystems
INSTALLS += target
