DESTDIR = $$QT_BUILD_TREE/plugins/phonon_backend
QT += phonon
TARGET = phonon_av
include(../../../qpluginbase.pri)

DEFINES += PHONON_MAKE_QT_ONLY_BACKEND

HEADERS += avaudiooutput.h \
           avbackend.h \
           avmediaobject.h

SOURCES += avaudiooutput.cpp \
           avbackend.cpp

OBJECTIVE_SOURCES += avmediaobject.mm

LIBS += -framework AVFoundation

target.path = $$[QT_INSTALL_PLUGINS]/phonon_backend
INSTALLS += target











