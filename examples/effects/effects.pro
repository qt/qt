TEMPLATE      = \
              subdirs
SUBDIRS       = \
              blurpicker \
              lighting \
              customshader

# install
target.path = $$[QT_INSTALL_EXAMPLES]/effects
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS effects.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/effects
INSTALLS += target sources

