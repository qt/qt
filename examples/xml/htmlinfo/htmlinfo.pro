SOURCES += main.cpp
QT -= gui

RESOURCES   = resources.qrc

win32: CONFIG += console

wince*|symbian:{
   htmlfiles.files = *.html
   htmlfiles.path = .
   DEPLOYMENT += htmlfiles
}

# install
target.path = $$[QT_INSTALL_EXAMPLES]/xml/htmlinfo
INSTALLS += target

maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)
