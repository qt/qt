HEADERS	    =   mainwindow.h \
		diagramitem.h \
		diagramscene.h \
		arrow.h \
		diagramtextitem.h
SOURCES	    =   mainwindow.cpp \
		diagramitem.cpp \
		main.cpp \
		arrow.cpp \
		diagramtextitem.cpp \
		diagramscene.cpp
RESOURCES   =	diagramscene.qrc


# install
target.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/diagramscene
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS diagramscene.pro images
sources.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/diagramscene
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

symbian: warning(This example might not fully work on Symbian platform)
maemo5: warning(This example might not fully work on Maemo platform)
simulator: warning(This example might not fully work on Simulator platform)
