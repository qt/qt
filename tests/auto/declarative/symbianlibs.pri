#additional libs required for orientation sensor
symbian {
    !contains(S60_VERSION, 3.1):!contains(S60_VERSION, 3.2) {
        LIBS += -lsensrvclient -lsensrvutil
    }
    contains(QT_CONFIG, s60): {
        LIBS += -lavkon -lcone
    }
}
