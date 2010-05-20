TEMPLATE = subdirs

SUBDIRS += folderlistmodel particles gestures

contains(QT_CONFIG, webkit): SUBDIRS += webkit

