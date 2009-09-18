HEADERS += \
    egl/qegl_p.h \
    egl/qeglproperties_p.h

SOURCES += \
    egl/qegl.cpp \
    egl/qeglproperties.cpp

contains(QT_CONFIG, wince*): SOURCES += egl/qegl_wince.cpp

unix {
    embedded {
        SOURCES += egl/qegl_qws.cpp
    } else {
        symbian {
            SOURCES += egl/qegl_symbian.cpp
        } else {
            SOURCES += egl/qegl_x11.cpp
        }
    }
}

for(p, QMAKE_LIBDIR_EGL) {
    exists($$p):LIBS_PRIVATE += -L$$p
}

!isEmpty(QMAKE_INCDIR_EGL): INCLUDEPATH += $$QMAKE_INCDIR_EGL
!isEmpty(QMAKE_LIBS_EGL): LIBS_PRIVATE += $$QMAKE_LIBS_EGL
!isEmpty(QMAKE_LFLAGS_EGL): LIBS += $$QMAKE_LFLAGS_EGL
