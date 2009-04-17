TEMPLATE      = subdirs
SUBDIRS       = \
                clockticking \
                composition \
                eventtransitions \
                factorial \
                helloworld \
                pauseandresume \
                pingpong \
                trafficlight \
                twowaybutton

# install
target.path = $$[QT_INSTALL_EXAMPLES]/statemachine
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS statemachine.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/statemachine
INSTALLS += target sources
