TARGET = qshivavggraphicssystem
include(../../qpluginbase.pri)

QT += openvg

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/graphicssystems

SOURCES = main.cpp shivavggraphicssystem.cpp shivavgwindowsurface.cpp
HEADERS = shivavggraphicssystem.h shivavgwindowsurface.h

target.path += $$[QT_INSTALL_PLUGINS]/graphicssystems
INSTALLS += target

!isEmpty(QMAKE_INCDIR_OPENVG): INCLUDEPATH += $$QMAKE_INCDIR_OPENVG
!isEmpty(QMAKE_LIBDIR_OPENVG): LIBS += -L$$QMAKE_LIBDIR_OPENVG
!isEmpty(QMAKE_LIBS_OPENVG): LIBS += $$QMAKE_LIBS_OPENVG
