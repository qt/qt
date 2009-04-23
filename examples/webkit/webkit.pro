TEMPLATE =  subdirs
SUBDIRS +=  formextractor \
            previewer \
            fancybrowser \
            googlechat

# install
target.path = $$[QT_INSTALL_EXAMPLES]/webkit
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS webkit.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/webkit
INSTALLS += target sources
