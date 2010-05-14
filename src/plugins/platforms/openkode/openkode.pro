TARGET = qopenkodeintegration
include(../../qpluginbase.pri)

QT += opengl

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms

SOURCES =   main.cpp \
            qopenkodeintegration.cpp \
            qopenkodewindowsurface.cpp \
            qopenkodewindow.cpp \
            qopenkodeglintegration.cpp

HEADERS =   qopenkodeintegration.h \
            qopenkodewindowsurface.h \
            qopenkodewindow.h \
            qopenkodeglintegration.h

RESOURCES = resources.qrc

target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target

LIBS += -lKD -lEGL
!isEmpty(QMAKE_INCDIR_OPENGL_ES2){
    INCLUDEPATH += $$QMAKE_INCDIR_OPENGL_ES2
}
!isEmpty(QMAKE_LIBDIR_OPENGL_ES2){
    for(p, QMAKE_LIBDIR_OPENGL_ES2) {
        exists($$p):LIBS += -L$$p
    }
}
!isEmpty(QMAKE_LIBS_OPENGL_ES2){
    LIBS += $$QMAKE_LIBS_OPENGL_ES2
} else {
    LIBS += -lGLESv2
}
