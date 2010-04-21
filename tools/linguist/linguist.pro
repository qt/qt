TEMPLATE = subdirs
SUBDIRS  = \
    lrelease \
    lupdate \
    lconvert
!no-png:!contains(QT_CONFIG, no-gui):SUBDIRS += linguist
