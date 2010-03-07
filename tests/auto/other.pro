# These tests don't nicely fit into one of the other .pro files.
# They are testing too many Qt modules at the same time.

TEMPLATE=subdirs
SUBDIRS=\
#           exceptionsafety_objects \ shouldn't enable it
           qaccessibility \
           qalgorithms \
           qcombobox \
           qcssparser \
           qdatastream \
           qdir \
           qfocusevent \
           qimage \
           qiodevice \
           qitemmodel \
           qlayout \
           qmdiarea \
           qmenu \
           qmenubar \
           qmouseevent \
           qpainter \
           qpixmap \
           qprinter \
           qsettings \
           qsplitter \
           qtabwidget \
           qtextbrowser \
           qtextdocument \
           qtextedit \
           qtoolbutton \
           qvariant \
           qwidget \
           qworkspace \
           windowsmobile

contains(QT_CONFIG, OdfWriter):SUBDIRS += qzip qtextodfwriter
mac: {
    SUBDIRS += macgui \
               macplist \
               qaccessibility_mac
}

embedded:!wince* {
    SUBDIRS += qcopchannel \
               qdirectpainter \
               qmultiscreen
}

symbian {
    SUBDIRS += qsoftkeymanager \
               qs60mainapplication
}

# Following tests depends on private API
!contains(QT_CONFIG, private_tests): SUBDIRS -= \
           qcssparser \

