TEMPLATE      = subdirs
SUBDIRS       = random_ai \
                spin_ai_with_error \
                spin_ai \
                seek_ai 

# install
target.path = $$[QT_INSTALL_EXAMPLES]/statemachine/tankgameplugins
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS tankgameplugins.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/statemachine/tankgameplugins
INSTALLS += target sources
