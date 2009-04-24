HEADERS     = window.h
SOURCES     = main.cpp \
              window.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/lineedits
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS lineedits.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/widgets/lineedits
INSTALLS += target sources

include($$QT_SOURCE_TREE/examples/examplebase.pri)

symbian:TARGET.UID3 = 0xA000C604