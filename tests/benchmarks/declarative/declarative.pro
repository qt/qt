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

contains(QT_CONFIG, opengl): SUBDIRS += painting qmlshadersplugin

!contains(QT_CONFIG, private_tests): SUBDIRS -= \
           creation \
           pointers \
           qdeclarativeimage \
           script \

include(../trusted-benchmarks.pri)
