

unix:!mac {
   contains(QT_CONFIG, pulseaudio) {
        DEFINES += QT_MULTIMEDIA_PULSEAUDIO
        HEADERS += $$PWD/qsoundeffect_pulse_p.h
        SOURCES += $$PWD/qsoundeffect_pulse_p.cpp
        QMAKE_CXXFLAGS += $$QT_CFLAGS_PULSEAUDIO
        LIBS += $$QT_LIBS_PULSEAUDIO
    } else {
        DEFINES += QT_MULTIMEDIA_QMEDIAPLAYER
        HEADERS += $$PWD/qsoundeffect_qmedia_p.h
        SOURCES += $$PWD/qsoundeffect_qmedia_p.cpp
    }
} else {
    HEADERS += $$PWD/qsoundeffect_qsound_p.h
    SOURCES += $$PWD/qsoundeffect_qsound_p.cpp
}

HEADERS += \
    $$PWD/qsoundeffect_p.h \
    $$PWD/wavedecoder_p.h

SOURCES += \
    $$PWD/qsoundeffect.cpp \
    $$PWD/wavedecoder_p.cpp
