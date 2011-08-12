HEADERS       = norwegianwoodstyle.h \
                widgetgallery.h
SOURCES       = main.cpp \
                norwegianwoodstyle.cpp \
                widgetgallery.cpp
RESOURCES     = styles.qrc

REQUIRES += "contains(styles, motif)"

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/styles
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS styles.pro images
sources.path = $$[QT_INSTALL_EXAMPLES]/widgets/styles
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

symbian: warning(This example might not fully work on Symbian platform)
maemo5: warning(This example might not fully work on Maemo platform)
simulator: warning(This example might not fully work on Simulator platform)
