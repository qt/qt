QT += opengl declarative testlib

TARGET = tst_performance

SOURCES += \
    tst_performance.cpp \
    ../../../../src/imports/shaders/shadereffectitem.cpp \
    ../../../../src/imports/shaders/shadereffectsource.cpp \
    ../../../../src/imports/shaders/shadereffect.cpp \
    ../../../../src/imports/shaders/shadereffectbuffer.cpp \
    ../../../../src/imports/shaders/scenegraph/qsggeometry.cpp

HEADERS += \
    ../../../../src/imports/shaders/shadereffectitem.h \
    ../../../../src/imports/shaders/shadereffectsource.h \
    ../../../../src/imports/shaders/shadereffect.h \
    ../../../../src/imports/shaders/shadereffectbuffer.h \
    ../../../../src/imports/shaders/scenegraph/qsggeometry.h

OTHER_FILES += \
    *.qml \
    *.png \
    *.jpg
