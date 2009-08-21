SOURCES += main.cpp blurpicker.cpp blureffect.cpp customshadereffect.cpp
HEADERS += blurpicker.h blureffect.h customshadereffect.h
RESOURCES += blurpicker.qrc
QT += opengl

# install
target.path = $$[QT_INSTALL_EXAMPLES]/effects/customshader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS customshader.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/effects/customshader
INSTALLS += target sources
