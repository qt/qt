TEMPLATE      = \
              subdirs
SUBDIRS       = \
              blurpicker \
              lighting

contains(QT_CONFIG, opengl)|contains(QT_CONFIG, opengles2):SUBDIRS += customshader

# install
target.path = $$[QT_INSTALL_EXAMPLES]/effects
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS effects.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/effects
INSTALLS += target sources

