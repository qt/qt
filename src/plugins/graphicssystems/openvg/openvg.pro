TARGET = qvggraphicssystem
include(../../qpluginbase.pri)

QT += openvg

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/graphicssystems

SOURCES = main.cpp qgraphicssystem_vg.cpp
HEADERS = qgraphicssystem_vg_p.h

target.path += $$[QT_INSTALL_PLUGINS]/graphicssystems
INSTALLS += target

!isEmpty(QMAKE_INCDIR_OPENVG): INCLUDEPATH += $$QMAKE_INCDIR_OPENVG
!isEmpty(QMAKE_LIBDIR_OPENVG): LIBS += -L$$QMAKE_LIBDIR_OPENVG
!isEmpty(QMAKE_LIBS_OPENVG): LIBS += $$QMAKE_LIBS_OPENVG

!isEmpty(QMAKE_INCDIR_EGL): INCLUDEPATH += $$QMAKE_INCDIR_EGL
!isEmpty(QMAKE_LIBDIR_EGL): LIBS += -L$$QMAKE_LIBDIR_EGL
!isEmpty(QMAKE_LIBS_EGL): LIBS += $$QMAKE_LIBS_EGL
