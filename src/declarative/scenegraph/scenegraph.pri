INCLUDEPATH += $$PWD/coreapi $$PWD/convenience $$PWD/3d
!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL

QT += opengl


# Core API
HEADERS += \
    $$PWD/coreapi/qsgdefaultrenderer_p.h \
    $$PWD/coreapi/qsggeometry.h \
    $$PWD/coreapi/qsgmaterial.h \
    $$PWD/coreapi/qsgmatrix4x4stack.h \
    $$PWD/coreapi/qsgmatrix4x4stack_p.h \
    $$PWD/coreapi/qsgnode.h \
    $$PWD/coreapi/qsgnodeupdater_p.h \
    $$PWD/coreapi/qsgrenderer_p.h
SOURCES += \
    $$PWD/coreapi/qsgdefaultrenderer.cpp \
    $$PWD/coreapi/qsggeometry.cpp \
    $$PWD/coreapi/qsgmaterial.cpp \
    $$PWD/coreapi/qsgmatrix4x4stack.cpp \
    $$PWD/coreapi/qsgnode.cpp \
    $$PWD/coreapi/qsgnodeupdater.cpp \
    $$PWD/coreapi/qsgrenderer.cpp


# Util API
HEADERS += \
    $$PWD/util/qsgareaallocator_p.h \
    $$PWD/util/qsgengine.h \
    $$PWD/util/qsgflatcolormaterial.h \
    $$PWD/util/qsgsimplerectnode.h \
    $$PWD/util/qsgsimpletexturenode.h \
    $$PWD/util/qsgtexturematerial.h \
    $$PWD/util/qsgtexturematerial_p.h \
    $$PWD/util/qsgvertexcolormaterial_p.h \
    $$PWD/util/qsgtexture.h \
    $$PWD/util/qsgtexture_p.h \
    $$PWD/util/qsgtextureprovider_p.h \
    $$PWD/util/qsgpainternode_p.h

SOURCES += \
    $$PWD/util/qsgareaallocator.cpp \
    $$PWD/util/qsgengine.cpp \
    $$PWD/util/qsgflatcolormaterial.cpp \
    $$PWD/util/qsgsimplerectnode.cpp \
    $$PWD/util/qsgsimpletexturenode.cpp \
    $$PWD/util/qsgtexturematerial.cpp \
    $$PWD/util/qsgvertexcolormaterial.cpp \
    $$PWD/util/qsgtexture.cpp \
    $$PWD/util/qsgtextureprovider.cpp \
    $$PWD/util/qsgpainternode.cpp


# QML / Adaptations API
HEADERS += \
    $$PWD/qsgadaptationlayer_p.h \
    $$PWD/qsgcontext_p.h \
    $$PWD/qsgcontextplugin_p.h \
    $$PWD/qsgdefaultglyphnode_p.h \
    $$PWD/qsgdistancefieldglyphcache_p.h \
    $$PWD/qsgdistancefieldglyphnode_p.h \
    $$PWD/qsgdistancefieldglyphnode_p_p.h \
    $$PWD/qsgdefaultglyphnode_p_p.h \
    $$PWD/qsgdefaultimagenode_p.h \
    $$PWD/qsgdefaultrectanglenode_p.h \
    $$PWD/qsgflashnode_p.h

SOURCES += \
    $$PWD/qsgadaptationlayer.cpp \
    $$PWD/qsgcontext.cpp \
    $$PWD/qsgcontextplugin.cpp \
    $$PWD/qsgdefaultglyphnode.cpp \
    $$PWD/qsgdefaultglyphnode_p.cpp \
    $$PWD/qsgdistancefieldglyphcache.cpp \
    $$PWD/qsgdistancefieldglyphnode.cpp \
    $$PWD/qsgdistancefieldglyphnode_p.cpp \
    $$PWD/qsgdefaultimagenode.cpp \
    $$PWD/qsgdefaultrectanglenode.cpp \
    $$PWD/qsgflashnode.cpp
