TEMPLATE      = subdirs
SUBDIRS       = \
                factorial \
                pingpong

!contains(QT_CONFIG, no-gui) {
    SUBDIRS +=  \
                eventtransitions \
                rogue \
                trafficlight \
                twowaybutton
}

# install
target.path = $$[QT_INSTALL_EXAMPLES]/statemachine
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS statemachine.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/statemachine
INSTALLS += target sources
