SOURCES   = addressbook.cpp \
            main.cpp
HEADERS   = addressbook.h

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tutorials/addressbook/part1
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS part1.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tutorials/addressbook/part1
INSTALLS += target sources

include($$QT_SOURCE_TREE/examples/examplebase.pri)
