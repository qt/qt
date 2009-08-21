HEADERS       = analogclock.h
SOURCES       = analogclock.cpp \
                main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/analogclock
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS analogclock.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/widgets/analogclock
INSTALLS += target sources

include($$QT_SOURCE_TREE/examples/examplebase.pri)

symbian:TARGET.UID3 = 0xA000A64F