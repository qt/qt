TEMPLATE = subdirs
SUBDIRS = \
        qhostinfo

!contains(QT_CONFIG, private_tests): SUBDIRS -= \
        qhostinfo
