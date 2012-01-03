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
