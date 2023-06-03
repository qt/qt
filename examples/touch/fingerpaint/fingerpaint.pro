HEADERS       = mainwindow.h \
                scribblearea.h
SOURCES       = main.cpp \
                mainwindow.cpp \
                scribblearea.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/touch/fingerpaint
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS fingerpaint.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/touch/fingerpaint
INSTALLS += target sources

maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

maemo5: warning(This example might not fully work on Maemo platform)
simulator: warning(This example might not fully work on Simulator platform)
