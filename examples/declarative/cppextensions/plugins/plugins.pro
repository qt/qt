TEMPLATE = lib
CONFIG += qt plugin
QT += declarative

DESTDIR = org/qtproject/TimeExample
TARGET  = qmlqtimeexampleplugin

SOURCES += plugin.cpp

qdeclarativesources.files += \
    org/qtproject/TimeExample/qmldir \
    org/qtproject/TimeExample/center.png \
    org/qtproject/TimeExample/clock.png \
    org/qtproject/TimeExample/Clock.qml \
    org/qtproject/TimeExample/hour.png \
    org/qtproject/TimeExample/minute.png

qdeclarativesources.path += $$[QT_INSTALL_EXAMPLES]/declarative/cppextensions/plugins/org/qtproject/TimeExample

sources.files += plugins.pro plugin.cpp plugins.qml README
sources.path += $$[QT_INSTALL_EXAMPLES]/declarative/cppextensions/plugins
target.path += $$[QT_INSTALL_EXAMPLES]/declarative/cppextensions/plugins/org/qtproject/TimeExample

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

INSTALLS += qdeclarativesources sources target

symbian {
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
    TARGET.EPOCALLOWDLLDATA = 1
}
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)
