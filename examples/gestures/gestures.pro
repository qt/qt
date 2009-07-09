TEMPLATE      = \
              subdirs
SUBDIRS       = \
              imageviewer \
              graphicsview \
              collidingmice

contains(QT_CONFIG, webkit) {
        SUBDIRS += pannablewebview
        contains(QT_CONFIG, svg):SUBDIRS += browser
}

# install
target.path = $$[QT_INSTALL_EXAMPLES]/gestures
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS gestures.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/gestures
INSTALLS += target sources
