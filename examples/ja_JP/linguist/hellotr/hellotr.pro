#! [0]
SOURCES      = main.cpp
#! [0] #! [1]
TRANSLATIONS = hellotr_ja.ts
#! [1]

# install
target.path = $$[QT_INSTALL_EXAMPLES]/linguist/hellotr
sources.files = $$SOURCES *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/linguist/hellotr
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

