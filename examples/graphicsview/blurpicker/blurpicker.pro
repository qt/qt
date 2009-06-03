SOURCES += main.cpp blurpicker.cpp blureffect.cpp
HEADERS += blurpicker.h blureffect.h
RESOURCES += blurpicker.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/blurpicker
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS blurpicker.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/blurpicker
INSTALLS += target sources
