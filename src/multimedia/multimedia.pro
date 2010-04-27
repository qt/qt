TEMPLATE = subdirs

contains(QT_CONFIG, multimedia) {
    SUBDIRS += multimedia

    contains(QT_CONFIG, mediaservices):SUBDIRS += mediaservices
}

