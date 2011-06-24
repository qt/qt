SOURCES += main.cpp \
           myinputpanel.cpp \
           myinputpanelcontext.cpp

HEADERS += myinputpanel.h \
           myinputpanelcontext.h

FORMS   += mainform.ui \
           myinputpanelform.ui

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tools/inputpanel
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS inputpanel.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tools/inputpanel
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

symbian: warning(This example might not fully work on Symbian platform)
maemo5: warning(This example might not fully work on Maemo platform)
simulator: warning(This example might not fully work on Simulator platform)
