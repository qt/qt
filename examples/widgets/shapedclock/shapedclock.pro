HEADERS       = shapedclock.h
SOURCES       = shapedclock.cpp \
                main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/shapedclock
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS shapedclock.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/widgets/shapedclock
INSTALLS += target sources

include($$QT_SOURCE_TREE/examples/examplebase.pri)

symbian:TARGET.UID3 = 0xA000C605