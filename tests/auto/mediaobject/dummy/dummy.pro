TEMPLATE = lib

isEmpty(QT_MAJOR_VERSION) {
   VERSION=4.6.4
} else {
   VERSION=$${QT_MAJOR_VERSION}.$${QT_MINOR_VERSION}.$${QT_PATCH_VERSION}
}
CONFIG += qt plugin

TARGET = phonon_dummy
DESTDIR = $$QT_BUILD_TREE/plugins/phonon_backend
DEPENDPATH += .
INCLUDEPATH += .

QT           += phonon


# Input
HEADERS += backend.h audiooutput.h mediaobject.h videowidget.h
SOURCES += backend.cpp audiooutput.cpp mediaobject.cpp videowidget.cpp

target.path = $$[QT_INSTALL_PLUGINS]/phonon_backend
INSTALLS += target
