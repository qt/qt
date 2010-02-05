
contains(QT_CONFIG, declarative) {
    QT += declarative

    system(pkg-config --exists \'libpulse >= 0.9.10\') {
        DEFINES += QT_MULTIMEDIA_PULSEAUDIO
        HEADERS += qsoundeffect_pulse_p.h
        SOURCES += qsoundeffect_pulse_p.cpp
        LIBS += -lpulse
    } else:x11 {
        DEFINES += QT_MULTIMEDIA_QMEDIAPLAYER
        HEADERS += qsoundeffect_qmedia_p.h
        SOURCES += qsoundeffect_qmedia_p.cpp
    } else {
        HEADERS += qsoundeffect_qsound_p.h
        SOURCES += qsoundeffect_qsound_p.cpp
    }

    HEADERS += \
        $$PWD/qmetadatacontrolmetaobject_p.h \
        $$PWD/qmlaudio_p.h \
        $$PWD/qmlgraphicsvideo_p.h \
        $$PWD/qmlmediabase_p.h \
        $$PWD/qsoundeffect_p.h \
        $$PWD/wavedecoder_p.h

    SOURCES += \
        $$PWD/qmetadatacontrolmetaobject.cpp \
        $$PWD/qmlaudio.cpp \
        $$PWD/qmlgraphicsvideo.cpp \
        $$PWD/qmlmediabase.cpp \
        $$PWD/qsoundeffect.cpp \
        $$PWD/wavedecoder_p.cpp
}

