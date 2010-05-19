TEMPLATE = subdirs

SUBDIRS += particles gestures

contains(QT_CONFIG, webkit): SUBDIRS += webkit

