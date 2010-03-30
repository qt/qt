TARGET = qsymbianbearer
include(../../qpluginbase.pri)

QT += network

HEADERS += symbianengine.h \
           qnetworksession_impl.h

SOURCES += symbianengine.cpp \
           qnetworksession_impl.cpp \
           main.cpp

symbian {
    exists($${EPOCROOT}epoc32/release/winscw/udeb/cmmanager.lib)| \
    exists($${EPOCROOT}epoc32/release/armv5/lib/cmmanager.lib) {
        message("Building with SNAP support")
        DEFINES += SNAP_FUNCTIONALITY_AVAILABLE
        LIBS += -lcmmanager
    } else {
        message("Building without SNAP support")
        LIBS += -lapengine
    }
}

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
symbian-abld:INCLUDEPATH += $$QT_BUILD_TREE/include/QtNetwork/private

LIBS += -lcommdb \
        -lapsettingshandlerui \
        -lconnmon \
        -lcentralrepository \
        -lesock \
        -linsock \
        -lecom \
        -lefsrv \
        -lnetmeta

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/bearer
target.path += $$[QT_INSTALL_PLUGINS]/bearer
INSTALLS += target
