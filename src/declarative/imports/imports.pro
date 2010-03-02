TEMPLATE = subdirs

SUBDIRS += widgets

contains(QT_CONFIG, multimedia): SUBDIRS += multimedia
contains(QT_CONFIG, webkit): SUBDIRS += webkit

