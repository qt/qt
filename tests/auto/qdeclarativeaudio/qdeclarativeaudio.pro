load(qttest_p4)

HEADERS += \
        $$PWD/../../../src/imports/multimedia/qdeclarativeaudio_p.h \
        $$PWD/../../../src/imports/multimedia/qdeclarativemediabase_p.h \
        $$PWD/../../../src/imports/multimedia/qmetadatacontrolmetaobject_p.h

SOURCES += \
        tst_qdeclarativeaudio.cpp \
        $$PWD/../../../src/imports/multimedia/qdeclarativeaudio.cpp \
        $$PWD/../../../src/imports/multimedia/qdeclarativemediabase.cpp \
        $$PWD/../../../src/imports/multimedia/qmetadatacontrolmetaobject.cpp

QT += mediaservices declarative
