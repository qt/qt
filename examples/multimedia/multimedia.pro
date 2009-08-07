TEMPLATE      = subdirs
SUBDIRS       = audio

# install
target.path = $$[QT_INSTALL_EXAMPLES]/multimedia
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS multimedia.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/multimedia
INSTALLS += target sources
