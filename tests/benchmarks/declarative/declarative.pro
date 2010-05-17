TEMPLATE = subdirs

SUBDIRS += \
           binding \
           creation \
           pointers \
           qdeclarativecomponent \
           qdeclarativeimage \
           qdeclarativemetaproperty \
           script \
           qmltime

contains(QT_CONFIG, opengl): SUBDIRS += painting


