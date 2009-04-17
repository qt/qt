TEMPLATE      = subdirs
SUBDIRS       = framebuffer mousecalibration simpledecoration

# install
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS README *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/qws
INSTALLS += sources
