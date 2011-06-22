HEADERS       = locationdialog.h \
                mainwindow.h \
                settingstree.h \
                variantdelegate.h
SOURCES       = locationdialog.cpp \
                main.cpp \
                mainwindow.cpp \
                settingstree.cpp \
                variantdelegate.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tools/settingseditor
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS settingseditor.pro inifiles
sources.path = $$[QT_INSTALL_EXAMPLES]/tools/settingseditor
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

symbian: warning(This example might not fully work on Symbian platform)
maemo5: warning(This example might not fully work on Maemo platform)
simulator: warning(This example might not fully work on Simulator platform)
