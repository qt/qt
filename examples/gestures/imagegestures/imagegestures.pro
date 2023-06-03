HEADERS   = imagewidget.h \
            mainwidget.h
SOURCES   = imagewidget.cpp \
            main.cpp \
            mainwidget.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/gestures/imagegestures
sources.files = $$SOURCES \
    $$HEADERS \
    $$RESOURCES \
    $$FORMS \
    imagegestures.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/gestures/imagegestures
INSTALLS += target \
    sources

maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)
maemo5: warning(This example does not work on Maemo platform)
simulator: warning(This example does not work on Simulator platform)
