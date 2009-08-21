TEMPLATE = subdirs
CONFIG  += ordered

SUBDIRS += contextsensitivehelp \
           remotecontrol \
           simpletextviewer

# install
sources.files = README *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/help
INSTALLS += sources

include($$QT_SOURCE_TREE/examples/examplebase.pri)
