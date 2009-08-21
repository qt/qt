SOURCES = testgui.cpp
CONFIG  += qtestlib

# install
target.path = $$[QT_INSTALL_EXAMPLES]/qtestlib/tutorial3
sources.files = $$SOURCES *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/qtestlib/tutorial3
INSTALLS += target sources

symbian {
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
    TARGET.UID3 = 0xA000C60D
}
