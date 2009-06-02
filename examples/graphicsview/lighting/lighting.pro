SOURCES += main.cpp lighting.cpp shadoweffect.cpp
HEADERS += lighting.h shadoweffect.h

# install
target.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/lighting
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS lighting.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/lighting
INSTALLS += target sources
