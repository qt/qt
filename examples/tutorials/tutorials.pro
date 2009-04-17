TEMPLATE      = subdirs
SUBDIRS       = \
                addressbook

# install
sources.files = README *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tutorials
INSTALLS += sources
