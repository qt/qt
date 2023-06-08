TEMPLATE = subdirs

SUBDIRS	*= sqldrivers script bearer
unix {
        contains(QT_CONFIG,iconv)|contains(QT_CONFIG,gnu-libiconv)|contains(QT_CONFIG,sun-libiconv):SUBDIRS *= codecs
} else {
        SUBDIRS *= codecs
}
!contains(QT_CONFIG, no-gui): SUBDIRS *= imageformats iconengines
!embedded:!qpa:!contains(QT_CONFIG, no-gui):SUBDIRS *= graphicssystems
embedded:SUBDIRS *=  gfxdrivers decorations mousedrivers kbddrivers
!win32:!embedded:!mac:!contains(QT_CONFIG, no-gui):SUBDIRS *= inputmethods
contains(QT_CONFIG, no-gui):SUBDIRS += accessible
qpa:SUBDIRS += platforms
contains(QT_CONFIG, declarative): SUBDIRS *= qmltooling
