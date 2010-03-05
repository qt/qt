load(qttest_p4)

HEADERS += \
        $$PWD/../../../src/plugins/qdeclarativemodules/multimedia/qdeclarativeaudio_p.h \
        $$PWD/../../../src/plugins/qdeclarativemodules/multimedia/qdeclarativemediabase_p.h \
        $$PWD/../../../src/plugins/qdeclarativemodules/multimedia/qmetadatacontrolmetaobject_p.h

SOURCES += \
        tst_qdeclarativeaudio.cpp \
        $$PWD/../../../src/plugins/qdeclarativemodules/multimedia/qdeclarativeaudio.cpp \
        $$PWD/../../../src/plugins/qdeclarativemodules/multimedia/qdeclarativemediabase.cpp \
        $$PWD/../../../src/plugins/qdeclarativemodules/multimedia/qmetadatacontrolmetaobject.cpp

QT += multimedia declarative
