TEMPLATE      = subdirs
SUBDIRS       = \
                eventtransitions \
                factorial \
                pingpong \
                trafficlight \
                twowaybutton \
                tankgame \
                tankgameplugins

# install
target.path = $$[QT_INSTALL_EXAMPLES]/statemachine
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS statemachine.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/statemachine
INSTALLS += target sources
