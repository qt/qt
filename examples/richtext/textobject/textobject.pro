HEADERS         = svgtextobject.h \
                  window.h
SOURCES         = main.cpp \
                  svgtextobject.cpp \
                  window.cpp

QT += svg

RESOURCES       = resources.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/richtext/textobject
sources.files = $$SOURCES $$HEADERS *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/richtext/textobject
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)
filesToDeploy.files = files/*.svg
filesToDeploy.path = files
DEPLOYMENT += filesToDeploy
