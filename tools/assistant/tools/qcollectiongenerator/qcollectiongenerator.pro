QT += xml \
    network
TEMPLATE = app
DESTDIR = ../../../../bin
TARGET = qcollectiongenerator
CONFIG += qt \
    warn_on \
    help \
    console
CONFIG -= app_bundle
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}
target.path = $$[QT_INSTALL_BINS]
INSTALLS += target
SOURCES += ../shared/helpgenerator.cpp \
    main.cpp \
    ../shared/collectionconfiguration.cpp
HEADERS += ../shared/helpgenerator.h \
    ../shared/collectionconfiguration.h

qclucene = QtCLucene$${QT_LIBINFIX}
if(!debug_and_release|build_pass):CONFIG(debug, debug|release) {
    mac:qclucene = $${qclucene}_debug
    win32:qclucene = $${qclucene}d
}
linux-lsb-g++:LIBS_PRIVATE += --lsb-shared-libs=$$qclucene
LIBS_PRIVATE += -l$$qclucene
