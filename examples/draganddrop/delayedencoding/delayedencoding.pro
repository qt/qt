QT          += svg

HEADERS     = mimedata.h \
              sourcewidget.h
RESOURCES   = delayedencoding.qrc
SOURCES     = main.cpp \
              mimedata.cpp \
              sourcewidget.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/draganddrop/delayedencoding
sources.files = $$SOURCES $$HEADERS *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/itemviews/delayedencoding
INSTALLS += target sources

symbian {
    TARGET.UID3 = 0xA000C614
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
}
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)
symbian: warning(This example does not work on Symbian platform)
maemo5: warning(This example does not work on Maemo platform)
simulator: warning(This example does not work on Simulator platform)
