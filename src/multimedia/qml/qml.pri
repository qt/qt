
contains(QT_CONFIG, declarative) {
    QT += declarative

    HEADERS += \
        $$PWD/qmlsound_p.h

    SOURCES += \
        $$PWD/qmlsound.cpp
}

