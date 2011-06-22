VPATH += ../shared
INCLUDEPATH += ../shared

QT += opengl
HEADERS = bubble.h \
    glwidget.h \
    qtlogo.h
SOURCES = bubble.cpp \
    glwidget.cpp \
    main.cpp \
    qtlogo.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl/overpainting
sources.files = $$SOURCES \
    $$HEADERS \
    $$RESOURCES \
    $$FORMS \
    overpainting.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/opengl/overpainting
INSTALLS += target \
    sources
symbian:include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

symbian: warning(This example might not fully work on Symbian platform)
maemo5: warning(This example does not work on Maemo platform)
simulator: warning(This example might not fully work on Simulator platform)
