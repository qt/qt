# Qt for Embedded Linux

embedded {
	CONFIG -= opengl x11
	LIBS -= -dl
	KERNEL_P        = kernel

	!mac:HEADERS += embedded/qsoundqss_qws.h
	HEADERS += \
		    embedded/qcopchannel_qws.h \
		    embedded/qdecoration_qws.h \
		    embedded/qdecorationfactory_qws.h \
		    embedded/qdecorationplugin_qws.h \
		    embedded/qdirectpainter_qws.h \
		    embedded/qlock_p.h \
		    embedded/qscreen_qws.h \
		    embedded/qscreenmulti_qws_p.h \
		    embedded/qscreenproxy_qws.h \
		    embedded/qwindowsystem_qws.h \
		    embedded/qwindowsystem_p.h \
		    embedded/qwscommand_qws_p.h \
		    embedded/qwscursor_qws.h \
		    embedded/qwsdisplay_qws.h \
		    embedded/qwsdisplay_qws_p.h \
		    embedded/qwsevent_qws.h \
		    embedded/qwsmanager_qws.h \
		    embedded/qwsmanager_p.h \
		    embedded/qwsproperty_qws.h \
		    embedded/qwsprotocolitem_qws.h \
                    embedded/qtransportauth_qws.h \
                    embedded/qtransportauth_qws_p.h \
                    embedded/qtransportauthdefs_qws.h \
		    embedded/qwssocket_qws.h \
		    embedded/qwslock_p.h \
		    embedded/qwsutils_qws.h \
                    embedded/qwssharedmemory_p.h \
		    embedded/qwssignalhandler_p.h \
		    embedded/qwsembedwidget.h

	 !mac:SOURCES += embedded/qsoundqss_qws.cpp
         SOURCES +=  \
		    embedded/qcopchannel_qws.cpp \
		    embedded/qdecoration_qws.cpp \
		    embedded/qdecorationfactory_qws.cpp \
		    embedded/qdecorationplugin_qws.cpp \
		    embedded/qdirectpainter_qws.cpp \
		    embedded/qlock.cpp \
		    embedded/qscreen_qws.cpp \
		    embedded/qscreenmulti_qws.cpp \
		    embedded/qscreenproxy_qws.cpp \
		    embedded/qwindowsystem_qws.cpp \
		    embedded/qwscommand_qws.cpp \
		    embedded/qwscursor_qws.cpp \
		    embedded/qwsevent_qws.cpp \
		    embedded/qwsmanager_qws.cpp \
		    embedded/qwsproperty_qws.cpp \
                    embedded/qtransportauth_qws.cpp \
		    embedded/qwslock.cpp \
                    embedded/qwssharedmemory.cpp \
		    embedded/qwssocket_qws.cpp \
		    embedded/qwssignalhandler.cpp \
		    embedded/qwsembedwidget.cpp

        contains(QT_CONFIG,sxe)|contains(QT_CONFIG,qtopia) {
            SOURCES += embedded/qunixsocket.cpp embedded/qunixsocketserver.cpp
            HEADERS += embedded/qunixsocket_p.h embedded/qunixsocketserver_p.h
        }

#
# Decorations
#
    contains( decorations, default ) {
    	HEADERS += embedded/qdecorationdefault_qws.h
    	SOURCES += embedded/qdecorationdefault_qws.cpp
    }
    contains( decorations, styled ) {
    	HEADERS += embedded/qdecorationstyled_qws.h
    	SOURCES += embedded/qdecorationstyled_qws.cpp
    }

    contains( decorations, windows ) {
    	HEADERS += embedded/qdecorationwindows_qws.h
    	SOURCES += embedded/qdecorationwindows_qws.cpp
    }

#
# Qt for Embedded Linux Drivers
#
	HEADERS += embedded/qscreendriverplugin_qws.h \
		    embedded/qscreendriverfactory_qws.h \
		    embedded/qkbd_qws.h \
		    embedded/qkbddriverplugin_qws.h \
		    embedded/qkbddriverfactory_qws.h \
		    embedded/qmouse_qws.h \
		    embedded/qmousedriverplugin_qws.h \
		    embedded/qmousedriverfactory_qws.h

	SOURCES += embedded/qscreendriverplugin_qws.cpp \
		    embedded/qscreendriverfactory_qws.cpp \
		    embedded/qkbd_qws.cpp \
		    embedded/qkbddriverplugin_qws.cpp \
		    embedded/qkbddriverfactory_qws.cpp \
		    embedded/qmouse_qws.cpp \
		    embedded/qmousedriverplugin_qws.cpp \
		    embedded/qmousedriverfactory_qws.cpp

#
# Graphics drivers
#
        contains( gfx-drivers, linuxfb ) {
	        HEADERS += embedded/qscreenlinuxfb_qws.h
		SOURCES += embedded/qscreenlinuxfb_qws.cpp
	}

	contains( gfx-drivers, qvfb ) {
		HEADERS += embedded/qscreenvfb_qws.h
		SOURCES += embedded/qscreenvfb_qws.cpp
	}

	contains( gfx-drivers, vnc ) {
		VNCDIR = $$QT_SOURCE_TREE/src/plugins/gfxdrivers/vnc
		INCLUDEPATH += $$VNCDIR
		HEADERS += $$VNCDIR/qscreenvnc_qws.h \
			   $$VNCDIR/qscreenvnc_p.h
		SOURCES += $$VNCDIR/qscreenvnc_qws.cpp
	}

	contains( gfx-drivers, transformed ) {
		HEADERS += embedded/qscreentransformed_qws.h
		SOURCES += embedded/qscreentransformed_qws.cpp
	}

#
# Keyboard drivers
#
	contains( kbd-drivers, qvfb ) {
		HEADERS +=embedded/qkbdvfb_qws.h
		SOURCES +=embedded/qkbdvfb_qws.cpp
		!contains( kbd-drivers, qvfb ) {
			kbd-drivers += qvfb
		}
         }

	contains( kbd-drivers, sl5000 ) {
		HEADERS +=embedded/qkbdsl5000_qws.h
		SOURCES +=embedded/qkbdsl5000_qws.cpp
		!contains( kbd-drivers, tty ) {
		    kbd-drivers += tty
		}
	}

	contains( kbd-drivers, tty ) {
		HEADERS +=embedded/qkbdtty_qws.h
		SOURCES +=embedded/qkbdtty_qws.cpp
		!contains( kbd-drivers, pc101 ) {
		    kbd-drivers += pc101
		}
	}

	contains( kbd-drivers, usb ) {
		HEADERS +=embedded/qkbdusb_qws.h
		SOURCES +=embedded/qkbdusb_qws.cpp
		!contains( kbd-drivers, pc101 ) {
		    kbd-drivers += pc101
		}
	}

	contains( kbd-drivers, um ) {
		HEADERS +=embedded/qkbdum_qws.h
		SOURCES +=embedded/qkbdum_qws.cpp
	}

	contains( kbd-drivers, pc101 ) {
		HEADERS +=embedded/qkbdpc101_qws.h
		SOURCES +=embedded/qkbdpc101_qws.cpp
	}

	contains( kbd-drivers, yopy ) {
		HEADERS +=embedded/qkbdyopy_qws.h
		SOURCES +=embedded/qkbdyopy_qws.cpp
	}

	contains( kbd-drivers, vr41xx ) {
		HEADERS +=embedded/qkbdvr41xx_qws.h
		SOURCES +=embedded/qkbdvr41xx_qws.cpp
	}

#
# Mouse drivers
#
	contains( mouse-drivers, qvfb ) {
		HEADERS +=embedded/qmousevfb_qws.h
		SOURCES +=embedded/qmousevfb_qws.cpp
	}

	contains( mouse-drivers, pc ) {
		HEADERS +=embedded/qmousepc_qws.h
		SOURCES +=embedded/qmousepc_qws.cpp
	}

	contains( mouse-drivers, bus ) {
		HEADERS +=embedded/qmousebus_qws.h
		SOURCES +=embedded/qmousebus_qws.cpp
	}

	contains( mouse-drivers, linuxtp ) {
		HEADERS +=embedded/qmouselinuxtp_qws.h
		SOURCES +=embedded/qmouselinuxtp_qws.cpp
	}

	contains( mouse-drivers, vr41xx ) {
		HEADERS +=embedded/qmousevr41xx_qws.h
		SOURCES +=embedded/qmousevr41xx_qws.cpp
	}

	contains( mouse-drivers, yopy ) {
		HEADERS +=embedded/qmouseyopy_qws.h
		SOURCES +=embedded/qmouseyopy_qws.cpp
	}
	
	contains( mouse-drivers, tslib ) {
		LIBS += -lts
		HEADERS +=embedded/qmousetslib_qws.h
		SOURCES +=embedded/qmousetslib_qws.cpp
	}
}
