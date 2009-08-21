TEMPLATE      = subdirs
SUBDIRS       = basiclayouts \
                borderlayout \
                dynamiclayouts \
                flowlayout

# install
sources.files = README *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/layouts
INSTALLS += sources

include($$QT_SOURCE_TREE/examples/examplebase.pri)
