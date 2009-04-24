SOURCES = testqstring.cpp
CONFIG  += qtestlib

# install
target.path = $$[QT_INSTALL_EXAMPLES]/qtestlib/tutorial2
sources.files = $$SOURCES *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/qtestlib/tutorial2
INSTALLS += target sources

include($$QT_SOURCE_TREE/examples/examplebase.pri)

symbian:TARGET.UID3 = 0xA000C60C