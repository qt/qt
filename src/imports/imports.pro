TEMPLATE = subdirs

SUBDIRS += folderlistmodel particles gestures
contains(QT_CONFIG, opengl):!contains(QT_CONFIG, opengles1): SUBDIRS += shaders

