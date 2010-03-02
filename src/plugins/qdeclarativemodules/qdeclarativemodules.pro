TEMPLATE = subdirs

SUBDIRS += widgets

contains(QT_CONFIG, multimedia): SUBDIRS += multimedia

