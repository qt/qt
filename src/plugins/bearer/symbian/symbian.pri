TARGET = qsymbianbearer
include(../../qpluginbase.pri)

QT += network

HEADERS += ../symbianengine.h \
           ../qnetworksession_impl.h

SOURCES += ../symbianengine.cpp \
           ../qnetworksession_impl.cpp \
           ../main.cpp

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
