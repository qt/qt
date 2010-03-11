TARGET  = multimedia
TARGETPATH = Qt/multimedia
include(../qimportbase.pri)

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

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/imports/$$TARGETPATH
target.path = $$TARGETPATH

qmldir.files += $$QT_BUILD_TREE/imports/Qt/multimedia/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir
