TARGET  = multimedia
include(../../qpluginbase.pri)

QT += multimedia declarative

HEADERS += \
        qdeclarativeaudio_p.h \
        qdeclarativemediabase_p.h \
        qdeclarativevideo_p.h \
        qmetadatacontrolmetaobject_p.h \

SOURCES += \
        multimedia.cpp \
        qdeclarativeaudio.cpp \
        qdeclarativemediabase.cpp \
        qdeclarativevideo.cpp \
        qmetadatacontrolmetaobject.cpp

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/imports/Qt/multimedia
target.path = $$[QT_INSTALL_IMPORTS]/Qt/multimedia

qmldir.files += $$QT_BUILD_TREE/imports/Qt/multimedia/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/Qt/multimedia

INSTALLS += target qmldir

