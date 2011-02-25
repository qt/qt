INCLUDEPATH += $$PWD/coreapi $$PWD/convenience $$PWD/3d
!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL

QT += opengl

# Core API
HEADERS += \
    $$PWD/coreapi/material.h \
    $$PWD/coreapi/node.h \
    $$PWD/coreapi/nodeupdater_p.h \
    $$PWD/coreapi/renderer.h \
    $$PWD/coreapi/qmlrenderer.h \
    $$PWD/coreapi/qsgcontext.h \
    $$PWD/coreapi/qsgcontextplugin.h \
    $$PWD/coreapi/qsgtexturemanager.h \
    $$PWD/coreapi/qsgtexturemanager_p.h \
    $$PWD/coreapi/qsgtextureprovider.h \
    scenegraph/coreapi/qsggeometry.h

SOURCES += \
    $$PWD/coreapi/material.cpp \
    $$PWD/coreapi/node.cpp \
    $$PWD/coreapi/nodeupdater.cpp \
    $$PWD/coreapi/renderer.cpp \
    $$PWD/coreapi/qmlrenderer.cpp \
    $$PWD/coreapi/qsgcontext.cpp \
    $$PWD/coreapi/qsgcontextplugin.cpp \
    $$PWD/coreapi/qsgtexturemanager.cpp \
    $$PWD/coreapi/qsgtextureprovider.cpp \
    scenegraph/coreapi/qsggeometry.cpp

# Convenience API
HEADERS += $$PWD/convenience/areaallocator.h \
    $$PWD/convenience/flatcolormaterial.h \
    $$PWD/convenience/solidrectnode.h \
    $$PWD/convenience/texturematerial.h \
    $$PWD/convenience/vertexcolormaterial.h \
    $$PWD/convenience/distancefieldfontatlas_p.h

SOURCES += $$PWD/convenience/areaallocator.cpp \
    $$PWD/convenience/flatcolormaterial.cpp \
    $$PWD/convenience/solidrectnode.cpp \
    $$PWD/convenience/texturematerial.cpp \
    $$PWD/convenience/vertexcolormaterial.cpp \
    $$PWD/convenience/distancefieldfontatlas.cpp


# 3D API (duplicates with qt3d)
HEADERS += \
    $$PWD/3d/qsgattributedescription.h \
    $$PWD/3d/qsgattributevalue.h \
    $$PWD/3d/qsgmatrix4x4stack.h \
    $$PWD/3d/qsgmatrix4x4stack_p.h \
    $$PWD/3d/qsgarray.h \


SOURCES += \
    $$PWD/3d/qsgarray.cpp \
    $$PWD/3d/qsgmatrix4x4stack.cpp \
    $$PWD/3d/qsgattributedescription.cpp \
    $$PWD/3d/qsgattributevalue.cpp \

include(adaptationlayers/adaptationlayers.pri)
