TEMPLATE = app
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += main.cpp imagescaling.cpp
HEADERS += imagescaling.h

# install
target.path = $$[QT_INSTALL_EXAMPLES]/qtconcurrent/imagescaling
sources.files = $$SOURCES $$HEADERS $$FORMS $$RESOURCES *.pro *.png
sources.path = $$[QT_INSTALL_EXAMPLES]/qtconcurrent/imagescaling
INSTALLS += target sources

include($$QT_SOURCE_TREE/examples/examplebase.pri)

wince*: DEPLOYMENT_PLUGIN += qgif qjpeg qtiff
