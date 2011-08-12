QT      +=  webkit network
HEADERS =   googlechat.h
SOURCES =   main.cpp \
            googlechat.cpp
FORMS = form.ui

# install
target.path = $$[QT_INSTALL_EXAMPLES]/webkit/googlechat
sources.files = $$SOURCES $$HEADERS $$FORMS *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/webkit/googlechat
INSTALLS += target sources

symbian {
    TARGET.UID3 = 0xA000CF6E
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
    TARGET.CAPABILITY = NetworkServices
}
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

symbian: warning(This example does not work on Symbian platform)
maemo5: warning(This example does not work on Maemo platform)
simulator: warning(This example does not work on Simulator platform)
