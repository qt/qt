TEMPLATE = subdirs

SUBDIRS	*= accessible imageformats sqldrivers iconengines script
unix:!symbian {
        contains(QT_CONFIG,iconv)|contains(QT_CONFIG,gnu-libiconv):SUBDIRS *= codecs
} else {
        SUBDIRS *= codecs s60
}
!embedded:SUBDIRS *= graphicssystems
embedded:SUBDIRS *=  gfxdrivers decorations mousedrivers kbddrivers
!win32:!embedded:!mac:!symbian:SUBDIRS *= inputmethods
contains(QT_CONFIG, phonon): SUBDIRS *= phonon
