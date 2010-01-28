
contains(QT_CONFIG, declarative) {
    QT += declarative

    HEADERS += \
        $$PWD/qmlsound_p.h \
        $$PWD/qmetadatacontrolmetaobject_p.h \
        $$PWD/qmlaudio_p.h \
        $$PWD/qmlgraphicsvideo_p.h \
        $$PWD/qmlmediabase_p.h

    SOURCES += \
        $$PWD/qmlsound.cpp \
        $$PWD/qmetadatacontrolmetaobject.cpp \
        $$PWD/qmlaudio.cpp \
        $$PWD/qmlgraphicsvideo.cpp \
        $$PWD/qmlmediabase.cpp
}

