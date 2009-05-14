DEPENDPATH += opengl
INCLUDEPATH += opengl
INCLUDEPATH += $$QMAKE_INCDIR_OPENGL
QT += opengl

contains(QT_CONFIG, opengles1) {
    SOURCES += gltexture.cpp \
               glsave.cpp 

    HEADERS += gltexture.h \
               glsave.h 
} else:contains(QT_CONFIG, opengles2) {
    SOURCES += gltexture.cpp \
               glbasicshaders.cpp \
               glsave.cpp

    HEADERS += gltexture.h \
               glbasicshaders.h \
               glsave.h
}
