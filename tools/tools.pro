TEMPLATE        = subdirs

!contains(QT_CONFIG, no-gui) {
    no-png {
        message("Some graphics-related tools are unavailable without PNG support")
    } else {
         SUBDIRS += assistant \
		    pixeltool \
		    porting \
                    qtestlib \
                    qttracereplay
         contains(QT_EDITION, Console) {
             SUBDIRS += designer/src/uitools     # Linguist depends on this
         } else {
             SUBDIRS += designer
         }
         symbian: SUBDIRS = designer
         wince*: SUBDIRS = qtestlib designer
         unix:!mac:!embedded:contains(QT_CONFIG, qt3support):SUBDIRS += qtconfig
         win32:!wince*:SUBDIRS += activeqt
    }
    contains(QT_CONFIG, declarative):SUBDIRS += qmlviewer qmldebugger
}

SUBDIRS     += linguist

mac {
    SUBDIRS += macdeployqt
}

embedded:SUBDIRS += kmap2qmap

contains(QT_CONFIG, declarative):SUBDIRS += qml
contains(QT_CONFIG, dbus):SUBDIRS += qdbus
!wince*:contains(QT_CONFIG, xmlpatterns): SUBDIRS += xmlpatterns xmlpatternsvalidator
embedded: SUBDIRS += makeqpf

!wince*:!cross_compile:SUBDIRS += qdoc3

CONFIG+=ordered
QTDIR_build:REQUIRES = "contains(QT_CONFIG, full-config)"
