symbian {
    HEADERS += window_owning_control.h

    contains(QT_CONFIG, s60): LIBS+= $$QMAKE_LIBS_S60
}
