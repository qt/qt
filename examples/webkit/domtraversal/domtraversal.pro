QT       += webkit network
FORMS     = window.ui
HEADERS   = window.h
SOURCES   = main.cpp \
            window.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/webkit/domtraversal
sources.files = $$SOURCES $$HEADERS $$FORMS $$RESOURCES *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/webkit/domtraversal
INSTALLS += target sources

symbian {
    TARGET.UID3 = 0xA000D7CB
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
}
