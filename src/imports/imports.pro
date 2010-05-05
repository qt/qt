TEMPLATE = subdirs

SUBDIRS += particles

contains(QT_CONFIG, webkit): SUBDIRS += webkit
contains(QT_CONFIG, mediaservices): SUBDIRS += multimedia

