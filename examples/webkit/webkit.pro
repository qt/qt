TEMPLATE =  subdirs
SUBDIRS +=  domtraversal \
            formextractor \
            previewer \
            fancybrowser \
            simpleselector \
            imageanalyzer \
            framecapture \
            simplewebplugin \
            webplugin \
            webftpclient

contains(QT_CONFIG, openssl):SUBDIRS += googlechat

# install
target.path = $$[QT_INSTALL_EXAMPLES]/webkit
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS webkit.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/webkit
INSTALLS += target sources

