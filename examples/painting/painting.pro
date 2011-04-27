TEMPLATE      = subdirs
SUBDIRS       = basicdrawing \
                concentriccircles \
                imagecomposition \
                painterpaths \
                transformations \
                fontsampler

contains(QT_CONFIG, svg): SUBDIRS += svggenerator svgviewer

# install
target.path = $$[QT_INSTALL_EXAMPLES]/painting
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS painting.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/painting
INSTALLS += target sources

