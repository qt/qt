TEMPLATE  = subdirs
SUBDIRS   = styledemo

contains(QT_CONFIG, svg) {
    SUBDIRS += embeddedsvgviewer \
               fluidlauncher
}

# install
sources.files = README *.pro
sources.path = $$[QT_INSTALL_DEMOS]/embedded
INSTALLS += sources

