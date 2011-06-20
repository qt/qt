QT += webkit network

HEADERS   = csvfactory.h \
            csvview.h \
            mainwindow.h

SOURCES   = csvfactory.cpp \
            csvview.cpp \
            main.cpp \
            mainwindow.cpp

RESOURCES = simplewebplugin.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/webkit/simplewebplugin
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/webkit/simplewebplugin
INSTALLS += target sources

symbian {
    TARGET.UID3 = 0xA000EFFF
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
}
