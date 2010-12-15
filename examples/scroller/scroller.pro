TEMPLATE      = subdirs
SUBDIRS       = graphicsview \
                plot \
                wheel

# install
sources.files = *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/scroller
INSTALLS += sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
