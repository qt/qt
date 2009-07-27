TEMPLATE  = subdirs
SUBDIRS   = styledemo

contains(QT_CONFIG, svg) {
    SUBDIRS += embeddedsvgviewer
    # no QProcess
    !vxworks:!qnx:SUBDIRS += fluidlauncher
}

# install
sources.files = README *.pro
sources.path = $$[QT_INSTALL_DEMOS]/embedded
INSTALLS += sources

