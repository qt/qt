DESTDIR = $$QT_BUILD_TREE/plugins/phonon_backend
QT += phonon
TARGET = phonon_mmf
PHONON_MMF_DIR = $$QT_SOURCE_TREE/src/3rdparty/phonon/mmf

# Input
HEADERS += \
           $$PHONON_MMF_DIR/abstractvideorenderer.h \
           $$PHONON_MMF_DIR/audiooutput.h \
           $$PHONON_MMF_DIR/backend.h \
           $$PHONON_MMF_DIR/backendnode.h \
           $$PHONON_MMF_DIR/effect.h \
           $$PHONON_MMF_DIR/fakesource.h \
           $$PHONON_MMF_DIR/iodevicereader.h \
           $$PHONON_MMF_DIR/mediagraph.h \
           $$PHONON_MMF_DIR/mediaobject.h \
           $$PHONON_MMF_DIR/videowidget.h \
           $$PHONON_MMF_DIR/videorenderer_soft.h \
           $$PHONON_MMF_DIR/videorenderer_vmr9.h \
           $$PHONON_MMF_DIR/volumeeffect.h \
           $$PHONON_MMF_DIR/qbasefilter.h \
           $$PHONON_MMF_DIR/qpin.h \
           $$PHONON_MMF_DIR/qasyncreader.h \
           $$PHONON_MMF_DIR/qaudiocdreader.h \
           $$PHONON_MMF_DIR/qmeminputpin.h \
           $$PHONON_MMF_DIR/compointer.h \
           $$PHONON_MMF_DIR/phononds9_namespace.h


SOURCES += \
           $$PHONON_MMF_DIR/abstractvideorenderer.cpp \
           $$PHONON_MMF_DIR/audiooutput.cpp \
           $$PHONON_MMF_DIR/backend.cpp \
           $$PHONON_MMF_DIR/backendnode.cpp \
           $$PHONON_MMF_DIR/effect.cpp \
           $$PHONON_MMF_DIR/fakesource.cpp \
           $$PHONON_MMF_DIR/iodevicereader.cpp \
           $$PHONON_MMF_DIR/mediagraph.cpp \
           $$PHONON_MMF_DIR/mediaobject.cpp \
           $$PHONON_MMF_DIR/videowidget.cpp \
           $$PHONON_MMF_DIR/videorenderer_soft.cpp \
           $$PHONON_MMF_DIR/videorenderer_vmr9.cpp \
           $$PHONON_MMF_DIR/volumeeffect.cpp \
           $$PHONON_MMF_DIR/qbasefilter.cpp \
           $$PHONON_MMF_DIR/qpin.cpp \
           $$PHONON_MMF_DIR/qasyncreader.cpp \
           $$PHONON_MMF_DIR/qaudiocdreader.cpp \
           $$PHONON_MMF_DIR/qmeminputpin.cpp


target.path = $$[QT_INSTALL_PLUGINS]/phonon_backend
INSTALLS += target

include(../../qpluginbase.pri)
