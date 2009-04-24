SOURCES += main.cpp
QT -= gui

wince*|symbian:{
   htmlfiles.sources = *.html
   htmlfiles.path = .
   DEPLOYMENT += htmlfiles
}

# install
target.path = $$[QT_INSTALL_EXAMPLES]/xml/htmlinfo
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.html htmlinfo.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/xml/htmlinfo
INSTALLS += target sources

include($$QT_SOURCE_TREE/examples/examplebase.pri)

symbian:TARGET.UID3 = 0xA000C609
