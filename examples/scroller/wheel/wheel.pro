HEADERS       = wheelwidget.h
SOURCES       = wheelwidget.cpp \
                main.cpp

QT += webkit

# install
target.path = $$[QT_INSTALL_EXAMPLES]/scroller/wheel
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS wheel.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/scroller/wheel
INSTALLS += target sources

symbian {
    TARGET.UID3 = 0xA000CF66
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
}
