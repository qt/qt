TEMPLATE = subdirs
SUBDIRS =

contains(QT_CONFIG, audio-backend) {
    symbian {
        SUBDIRS += symbian
    }
}

