TEMPLATE = app

CONFIG += help

SOURCES += main.cpp \
           wateringconfigdialog.cpp \
           helpbrowser.cpp

HEADERS += wateringconfigdialog.h \
           helpbrowser.h

FORMS   += wateringconfigdialog.ui

# install
target.path = $$[QT_INSTALL_EXAMPLES]/help/contextsensitivehelp
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.pro *.png *.doc doc
sources.path = $$[QT_INSTALL_EXAMPLES]/help/contextsensitivehelp
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

symbian: warning(This example does not work on Symbian platform)
maemo5: warning(This example does not work on Maemo platform)
simulator: warning(This example does not work on Simulator platform)

qclucene = QtCLucene$${QT_LIBINFIX}
if(!debug_and_release|build_pass):CONFIG(debug, debug|release) {
    mac:qclucene = $${qclucene}_debug
    win32:qclucene = $${qclucene}d
}
linux-lsb-g++:LIBS_PRIVATE += --lsb-shared-libs=$$qclucene
LIBS_PRIVATE += -l$$qclucene
