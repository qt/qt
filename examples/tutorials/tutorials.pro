TEMPLATE      = subdirs
SUBDIRS       = \
                addressbook

# install
sources.files = README *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tutorials
INSTALLS += sources

include($$QT_SOURCE_TREE/examples/examplebase.pri)
