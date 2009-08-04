TEMPLATE  = subdirs
SUBDIRS   = styledemo

contains(QT_CONFIG, svg) {
    SUBDIRS += embeddedsvgviewer \
               fluidlauncher \
               desktopservices
}

contains(QT_CONFIG, webkit) {
    SUBDIRS += anomaly
}

# install
sources.files = README *.pro
sources.path = $$[QT_INSTALL_DEMOS]/embedded
INSTALLS += sources

include($$QT_SOURCE_TREE/demos/demobase.pri)
