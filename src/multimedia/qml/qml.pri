
contains(QT_CONFIG, declarative) {
    QT += declarative

    system(pkg-config --exists \'libpulse >= 0.9.10\') {
        DEFINES += QT_MULTIMEDIA_PULSEAUDIO
        HEADERS += $$PWD/qsoundeffect_pulse_p.h
        SOURCES += $$PWD/qsoundeffect_pulse_p.cpp
        LIBS += -lpulse
    } else:x11 {
        DEFINES += QT_MULTIMEDIA_QMEDIAPLAYER
        HEADERS += $$PWD/qsoundeffect_qmedia_p.h
        SOURCES += $$PWD/qsoundeffect_qmedia_p.cpp
    } else {
        HEADERS += $$PWD/qsoundeffect_qsound_p.h
        SOURCES += $$PWD/qsoundeffect_qsound_p.cpp
    }

    HEADERS += \
        $$PWD/multimediadeclarative.h \
        $$PWD/qmetadatacontrolmetaobject_p.h \
        $$PWD/qdeclarativeaudio_p.h \
        $$PWD/qdeclarativevideo_p.h \
        $$PWD/qdeclarativemediabase_p.h \
        $$PWD/qsoundeffect_p.h \
        $$PWD/wavedecoder_p.h

    SOURCES += \
        $$PWD/multimediadeclarative.cpp \
        $$PWD/qmetadatacontrolmetaobject.cpp \
        $$PWD/qdeclarativeaudio.cpp \
        $$PWD/qdeclarativevideo.cpp \
        $$PWD/qdeclarativemediabase.cpp \
        $$PWD/qsoundeffect.cpp \
        $$PWD/wavedecoder_p.cpp
}

