TEMPLATE      = subdirs
SUBDIRS       = imagescaling \
                map \
                progressdialog \
                runfunction \
                wordcount

# install
target.path = $$[QT_INSTALL_EXAMPLES]/qtconcurrent
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS qtconcurrent.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/qtconcurrent
INSTALLS += target sources

include($$QT_SOURCE_TREE/examples/examplebase.pri)
