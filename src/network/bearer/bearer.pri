# Qt network bearer management module

#DEFINES += BEARER_MANAGEMENT_DEBUG

HEADERS += bearer/qnetworkconfiguration.h \
           bearer/qnetworksession.h \
           bearer/qnetworkconfigmanager.h \
           bearer/qbearerplugin.h

SOURCES += bearer/qnetworksession.cpp \
           bearer/qnetworkconfigmanager.cpp \
           bearer/qnetworkconfiguration.cpp \
           bearer/qbearerplugin.cpp

symbian {
    exists($${EPOCROOT}epoc32/release/winscw/udeb/cmmanager.lib)| \
    exists($${EPOCROOT}epoc32/release/armv5/lib/cmmanager.lib) {
        message("Building with SNAP support")
        DEFINES += SNAP_FUNCTIONALITY_AVAILABLE
        LIBS += -lcmmanager
    } else {
        message("Building without SNAP support")
        LIBS += -lapengine
    }
    
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    HEADERS += bearer/qnetworkconfigmanager_s60_p.h \
               bearer/qnetworkconfiguration_s60_p.h \
               bearer/qnetworksession_s60_p.h
    SOURCES += bearer/qnetworkconfigmanager_s60_p.cpp \
               bearer/qnetworkconfiguration_s60_p.cpp \
               bearer/qnetworksession_s60_p.cpp
               
    LIBS += -lcommdb \
            -lapsettingshandlerui \
            -lconnmon \
            -lcentralrepository \
            -lesock \
            -linsock \
            -lecom \
            -lefsrv \
            -lnetmeta
} else:maemo {
    QT += dbus
    CONFIG += link_pkgconfig

    exists(../debug) {
        message("Enabling debug messages.")
        DEFINES += BEARER_MANAGEMENT_DEBUG
    }

    HEADERS += bearer/qnetworksession_maemo_p.h \
               bearer/qnetworkconfigmanager_maemo_p.h \
               bearer/qnetworkconfiguration_maemo_p.h

    SOURCES += bearer/qnetworkconfigmanager_maemo.cpp \
               bearer/qnetworksession_maemo.cpp

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

    HEADERS += bearer/qnetworkconfigmanager_p.h \
               bearer/qnetworkconfiguration_p.h \
               bearer/qnetworksession_p.h \
               bearer/qnetworksessionengine_p.h

    SOURCES += bearer/qnetworkconfigmanager_p.cpp \
               bearer/qnetworksession_p.cpp \
               bearer/qnetworksessionengine.cpp

    contains(QT_CONFIG, networkmanager):DEFINES += BACKEND_NM
}

