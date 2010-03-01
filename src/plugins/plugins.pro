TEMPLATE = subdirs

SUBDIRS	*= imageformats sqldrivers iconengines script bearer
unix:!symbian {
        contains(QT_CONFIG,iconv)|contains(QT_CONFIG,gnu-libiconv):SUBDIRS *= codecs
} else {
        SUBDIRS *= codecs
}
!embedded:SUBDIRS *= graphicssystems
embedded:SUBDIRS *=  gfxdrivers decorations mousedrivers kbddrivers
!win32:!embedded:!mac:!symbian:SUBDIRS *= inputmethods
!symbian:SUBDIRS += accessible
symbian:SUBDIRS += s60
contains(QT_CONFIG, phonon): SUBDIRS *= phonon
contains(QT_CONFIG, multimedia): SUBDIRS *= audio mediaservices
contains(QT_CONFIG, declarative): SUBDIRS *= qdeclarativemodules


