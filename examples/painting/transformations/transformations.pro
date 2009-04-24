HEADERS     = renderarea.h \
              window.h
SOURCES     = main.cpp \
              renderarea.cpp \
	      window.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/painting/transformations
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS transformations.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/painting/transformations
INSTALLS += target sources

include($$QT_SOURCE_TREE/examples/examplebase.pri)

symbian:TARGET.UID3 = 0xA000A64D