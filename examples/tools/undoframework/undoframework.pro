HEADERS	    = commands.h \
	      diagramitem.h \
	      diagramscene.h \
	      mainwindow.h
SOURCES	    = commands.cpp \
              diagramitem.cpp \
              diagramscene.cpp \
              main.cpp \
              mainwindow.cpp
RESOURCES   = undoframework.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tools/undoframework
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS undoframework.pro README images
sources.path = $$[QT_INSTALL_EXAMPLES]/tools/undoframework
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

symbian: warning(This example might not fully work on Symbian platform)
maemo5: warning(This example might not fully work on Maemo platform)
simulator: warning(This example might not fully work on Simulator platform)
