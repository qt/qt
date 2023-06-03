TEMPLATE        = subdirs

!contains(QT_CONFIG, no-gui) {
    no-png {
        message("Some graphics-related tools are unavailable without PNG support")
    } else {
        wince* {
            SUBDIRS = qtestlib designer
        } else {
            SUBDIRS = assistant \
                      pixeltool \
                      qtestlib \
                      qttracereplay
            contains(QT_EDITION, Console) {
                SUBDIRS += designer/src/uitools     # Linguist depends on this
            } else {
                SUBDIRS += designer
            }
        }
        unix:!mac:!embedded:!qpa:SUBDIRS += qtconfig
        win32:!wince*:SUBDIRS += activeqt
    }
    contains(QT_CONFIG, declarative) {
        SUBDIRS += qml
        !wince*: SUBDIRS += qmlplugindump
    }
}

!wince*:SUBDIRS += linguist

mac {
    SUBDIRS += macdeployqt
}

embedded:SUBDIRS += kmap2qmap

contains(QT_CONFIG, dbus):SUBDIRS += qdbus
# We don't need these command line utilities on embedded platforms.
!wince*:contains(QT_CONFIG, xmlpatterns): SUBDIRS += xmlpatterns xmlpatternsvalidator
embedded: SUBDIRS += makeqpf

!wince*:!cross_compile:SUBDIRS += qdoc3

CONFIG+=ordered
QTDIR_build:REQUIRES = "contains(QT_CONFIG, full-config)"

!win32:!embedded:!mac:CONFIG += x11

x11:contains(QT_CONFIG, opengles2):contains(QT_CONFIG, egl):SUBDIRS += qmeegographicssystemhelper
