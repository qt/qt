TEMPLATE      = subdirs
SUBDIRS       = htmlinfo \
                xmlstreamlint

!contains(QT_CONFIG, no-gui) {
    SUBDIRS +=  dombookmarks \
                rsslisting \
                saxbookmarks \
                streambookmarks
}

symbian: SUBDIRS = htmlinfo saxbookmarks

# install
target.path = $$[QT_INSTALL_EXAMPLES]/xml
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS xml.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/xml
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
