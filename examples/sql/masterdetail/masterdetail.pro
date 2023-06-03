HEADERS   = database.h \
            dialog.h \
            mainwindow.h
RESOURCES = masterdetail.qrc
SOURCES   = dialog.cpp \
            main.cpp \
            mainwindow.cpp

QT += sql
QT += xml

# install
target.path = $$[QT_INSTALL_EXAMPLES]/sql/masterdetail
sources.files = $$SOURCES *.h $$RESOURCES $$FORMS masterdetail.pro *.xml images
sources.path = $$[QT_INSTALL_EXAMPLES]/sql/masterdetail
INSTALLS += target sources

maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

maemo5: warning(This example might not fully work on Maemo platform)
simulator: warning(This example might not fully work on Simulator platform)
