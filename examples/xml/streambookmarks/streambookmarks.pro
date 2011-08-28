HEADERS       = mainwindow.h \
                xbelreader.h \
                xbelwriter.h
SOURCES       = main.cpp \
                mainwindow.cpp \
                xbelreader.cpp \
                xbelwriter.cpp
QT           += xml

# install
target.path = $$[QT_INSTALL_EXAMPLES]/xml/streambookmarks
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS streambookmarks.pro *.xbel
sources.path = $$[QT_INSTALL_EXAMPLES]/xml/streambookmarks
INSTALLS += target sources

symbian: {
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
    addFiles.files = frank.xbel jennifer.xbel
    addFiles.path = /data/qt/streambookmarks
    DEPLOYMENT += addFiles
}
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

