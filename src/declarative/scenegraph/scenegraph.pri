INCLUDEPATH += $$PWD/coreapi $$PWD/convenience $$PWD/3d
!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL



QT += opengl

# Core API
HEADERS += $$PWD/coreapi/geometry.h \
    $$PWD/coreapi/material.h \
    $$PWD/coreapi/node.h \
    $$PWD/coreapi/nodeupdater_p.h \
    $$PWD/coreapi/renderer.h \
    $$PWD/coreapi/qmlrenderer.h \
    $$PWD/coreapi/qsgcontext.h \
    $$PWD/coreapi/qsgtexturemanager.h \
    $$PWD/coreapi/qsgtexturemanager_p.h \
    $$PWD/coreapi/qsgcontext.h

SOURCES += $$PWD/coreapi/geometry.cpp \
    $$PWD/coreapi/material.cpp \
    $$PWD/coreapi/node.cpp \
    $$PWD/coreapi/nodeupdater.cpp \
    $$PWD/coreapi/renderer.cpp \
    $$PWD/coreapi/qmlrenderer.cpp \
    $$PWD/coreapi/qsgcontext.h \
    $$PWD/coreapi/qsgtexturemanager.cpp \
    $$PWD/coreapi/qsgcontext.cpp


# Convenience API
HEADERS += $$PWD/convenience/areaallocator.h \
    $$PWD/convenience/flatcolormaterial.h \
    $$PWD/convenience/solidrectnode.h \
    $$PWD/convenience/texturematerial.h \
    $$PWD/convenience/utilities.h \
    $$PWD/convenience/vertexcolormaterial.h \

SOURCES += $$PWD/convenience/areaallocator.cpp \
    $$PWD/convenience/flatcolormaterial.cpp \
    $$PWD/convenience/solidrectnode.cpp \
    $$PWD/convenience/texturematerial.cpp \
    $$PWD/convenience/utilities.cpp \
    $$PWD/convenience/vertexcolormaterial.cpp \


# 3D API (duplicates with qt3d)
HEADERS += \
    $$PWD/3d/qglnamespace.h \
    $$PWD/3d/qt3dglobal.h \
    $$PWD/3d/qglattributedescription.h \
    $$PWD/3d/qglattributevalue.h \
    $$PWD/3d/qmatrix4x4stack.h \
    $$PWD/3d/qmatrix4x4stack_p.h \
    $$PWD/3d/qarray.h \
    $$PWD/3d/qcustomdataarray.h \
    $$PWD/3d/qcolor4ub.h \


SOURCES += \
    $$PWD/3d/qarray.cpp \
    $$PWD/3d/qmatrix4x4stack.cpp \
    $$PWD/3d/qglattributedescription.cpp \
    $$PWD/3d/qglattributevalue.cpp \
    $$PWD/3d/qglnamespace.cpp \
    $$PWD/3d/qcustomdataarray.cpp \
    $$PWD/3d/qcolor4ub.cpp \

!win32: !mac {
    INCLUDEPATH += $$PWD/../../3rdparty/freetype/include
}

contains(DEFINES, QML_DISTANCE_FIELDS): {
    HEADERS += $$PWD/convenience/distancefieldfontatlas_p.h
    SOURCES += $$PWD/convenience/distancefieldfontatlas.cpp
}



include(adaptationlayers/adaptationlayers.pri)
