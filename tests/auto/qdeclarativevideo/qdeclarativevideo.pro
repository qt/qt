load(qttest_p4)

HEADERS += \
        $$PWD/../../../src/imports/multimedia/qdeclarativevideo_p.h \
        $$PWD/../../../src/imports/multimedia/qdeclarativemediabase_p.h \
        $$PWD/../../../src/imports/multimedia/qmetadatacontrolmetaobject_p.h

SOURCES += \
        tst_qdeclarativevideo.cpp \
        $$PWD/../../../src/imports/multimedia/qdeclarativevideo.cpp \
        $$PWD/../../../src/imports/multimedia/qdeclarativemediabase.cpp \
        $$PWD/../../../src/imports/multimedia/qmetadatacontrolmetaobject.cpp

QT += multimedia mediaservices declarative
