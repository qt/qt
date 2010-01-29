# Qt bearer management library
TEMPLATE = lib
TARGET = QtBearer

QT += network
include (../../common.pri)

DEFINES += QT_BUILD_BEARER_LIB QT_MAKEDLL

#DEFINES += BEARER_MANAGEMENT_DEBUG

PUBLIC_HEADERS += qnetworkconfiguration.h \
           qnetworksession.h \
           qnetworkconfigmanager.h

HEADERS += $$PUBLIC_HEADERS
SOURCES += qnetworksession.cpp \
           qnetworkconfigmanager.cpp \
           qnetworkconfiguration.cpp

symbian: {
    contains(snap_enabled, yes) {
        message("Building with SNAP support")
        DEFINES += SNAP_FUNCTIONALITY_AVAILABLE=1
        LIBS += -lcmmanager
    } else {
        message("Building without SNAP support")
	LIBS += -lapengine
    }
    
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    HEADERS += qnetworkconfigmanager_s60_p.h \
               qnetworkconfiguration_s60_p.h \
               qnetworksession_s60_p.h
    SOURCES += qnetworkconfigmanager_s60_p.cpp \
               qnetworkconfiguration_s60_p.cpp \
               qnetworksession_s60_p.cpp
               
    LIBS += -lcommdb \
            -lapsettingshandlerui \
            -lconnmon \
            -lcentralrepository \
            -lesock \
            -linsock \
            -lecom \
            -lefsrv \
            -lnetmeta

    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID3 = 0x2002AC81
            
    QtBearerManagement.sources = QtBearer.dll
    QtBearerManagement.path = /sys/bin
    DEPLOYMENT += QtBearerManagement
} else {
    maemo6 {
	CONFIG += link_pkgconfig

	exists(../debug) {
		message("Enabling debug messages.")
		DEFINES += BEARER_MANAGEMENT_DEBUG
	}

        HEADERS += qnetworksession_maemo_p.h \
                   qnetworkconfigmanager_maemo_p.h \
                   qnetworkconfiguration_maemo_p.h

        SOURCES += qnetworkconfigmanager_maemo.cpp \
		   qnetworksession_maemo.cpp

	documentation.path = $$QT_MOBILITY_PREFIX/doc
        documentation.files = doc/html

	PKGCONFIG += glib-2.0 dbus-glib-1 gconf-2.0 osso-ic conninet

	CONFIG += create_pc create_prl
	QMAKE_PKGCONFIG_REQUIRES = glib-2.0 dbus-glib-1 gconf-2.0 osso-ic conninet
	pkgconfig.path = $$QT_MOBILITY_LIB/pkgconfig
	pkgconfig.files = QtBearer.pc

	INSTALLS += pkgconfig documentation

    } else {

        DEFINES += BEARER_ENGINE

        HEADERS += qnetworkconfigmanager_p.h \
                   qnetworkconfiguration_p.h \
                   qnetworksession_p.h \
                   qnetworksessionengine_p.h \
                   qgenericengine_p.h

        SOURCES += qnetworkconfigmanager_p.cpp \
                   qnetworksession_p.cpp \
                   qnetworksessionengine.cpp \
                   qgenericengine.cpp

        unix:!mac:contains(networkmanager_enabled, yes) {
            contains(QT_CONFIG,dbus) {
                DEFINES += BACKEND_NM
                QT += dbus

                HEADERS += qnmdbushelper_p.h \
                           qnetworkmanagerservice_p.h \
                           qnmwifiengine_unix_p.h

                SOURCES += qnmdbushelper.cpp \
                           qnetworkmanagerservice_p.cpp \
                           qnmwifiengine_unix.cpp
            } else {
                message("NetworkManager backend requires Qt DBus support")
            }
        }

        win32: {
            HEADERS += qnlaengine_win_p.h \
                   qnetworksessionengine_win_p.h

            !wince*:HEADERS += qnativewifiengine_win_p.h

            SOURCES += qnlaengine_win.cpp

            !wince*:SOURCES += qnativewifiengine_win.cpp

            !wince*:LIBS += -lWs2_32
            wince*:LIBS += -lWs2
        }
    }
    macx: {
        HEADERS += qcorewlanengine_mac_p.h
        SOURCES+= qcorewlanengine_mac.mm
        LIBS += -framework Foundation -framework SystemConfiguration

            contains(corewlan_enabled, yes) {
                     isEmpty(QMAKE_MAC_SDK) {
                         SDK6="yes"
                     } else {
                         contains(QMAKE_MAC_SDK, "/Developer/SDKs/MacOSX10.6.sdk") {
                             SDK6="yes"
                     }     
                 }
            
                !isEmpty(SDK6) {
                        LIBS += -framework CoreWLAN
                        DEFINES += MAC_SDK_10_6
                }
           }


    }
}

CONFIG += middleware
include(../../features/deploy.pri)
