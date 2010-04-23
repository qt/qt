TARGET = qopenkodeintegration
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms

SOURCES =   main.cpp \
            qopenkodeintegration.cpp \
            qopenkodewindowsurface.cpp \
            qopenkodewindow.cpp

HEADERS =   qopenkodeintegration.h \
            qopenkodewindowsurface.h \
            qopenkodewindow.h

RESOURCES = resources.qrc

target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target

LIBS += -lKD -lEGL -lGLESv2 
