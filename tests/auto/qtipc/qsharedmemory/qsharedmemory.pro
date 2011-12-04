TEMPLATE = subdirs
SUBDIRS = test qsystemlock

!contains(QT_CONFIG, private_tests): SUBDIRS -= \
          test \
          qsystemlock \
