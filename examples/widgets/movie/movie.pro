HEADERS     = movieplayer.h
SOURCES     = main.cpp \
              movieplayer.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/movie
sources.files = $$SOURCES $$HEADERS $$RESOURCES movie.pro animation.mng
sources.path = $$[QT_INSTALL_EXAMPLES]/widgets/movie
INSTALLS += target sources

include($$QT_SOURCE_TREE/examples/examplebase.pri)

wince*: {
   addFiles.sources += *.mng
   addFiles.path = .
   DEPLOYMENT += addFiles
   DEPLOYMENT_PLUGIN += qmng
}

