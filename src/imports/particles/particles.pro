TARGET  = particles
TARGETPATH = Qt/labs/particles
include(../qimportbase.pri)

QT += declarative

SOURCES += \
    qdeclarativeparticles.cpp \
    particles.cpp

HEADERS += \
    qdeclarativeparticles_p.h

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/imports/$$TARGETPATH
target.path = $$TARGETPATH

# install qmldir file
qmldir.files += qmldir
qmldir.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir
