# Input
SOURCES += main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/animation/moveblocks
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS states.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/animation/moveblocks
INSTALLS += target sources
