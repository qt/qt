TEMPLATE      = subdirs
SUBDIRS       = audioinput \
                audiooutput \
                audiodevices

# install
target.path = $$[QT_INSTALL_EXAMPLES]/multimedia/audio
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS audio.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/multimedia/audio
INSTALLS += target sources
