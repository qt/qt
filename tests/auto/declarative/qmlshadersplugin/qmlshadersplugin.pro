load(qttest_p4)

QT += opengl declarative
SOURCES += tst_qmlshadersplugin.cpp

SOURCES += \
    ../../../../src/imports/shaders/src/shadereffectitem.cpp \
    ../../../../src/imports/shaders/src/shadereffectsource.cpp \
    ../../../../src/imports/shaders/src/shadereffect.cpp \
    ../../../../src/imports/shaders/src/shadereffectbuffer.cpp \
    ../../../../src/imports/shaders/src/scenegraph/qsggeometry.cpp

HEADERS += \
    ../../../../src/imports/shaders/src/shadereffectitem.h \
    ../../../../src/imports/shaders/src/shadereffectsource.h \
    ../../../../src/imports/shaders/src/shadereffect.h \
    ../../../../src/imports/shaders/src/shadereffectbuffer.h \
    ../../../../src/imports/shaders/src/scenegraph/qsggeometry.h
