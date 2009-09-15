symbian {
    contains(QT_CONFIG, s60): LIBS+= $$QMAKE_LIBS_S60
    RESOURCES += symbian/symbianresources.qrc
}
