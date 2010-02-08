# Qt network bearer management module

#DEFINES += BEARER_MANAGEMENT_DEBUG

HEADERS += bearer/qnetworkconfiguration.h \
           bearer/qnetworksession.h \
           bearer/qnetworkconfigmanager.h

SOURCES += bearer/qnetworksession.cpp \
           bearer/qnetworkconfigmanager.cpp \
           bearer/qnetworkconfiguration.cpp

maemo {
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
    HEADERS += bearer/qnetworkconfigmanager_p.h \
               bearer/qnetworkconfiguration_p.h \
               bearer/qnetworksession_p.h \
               bearer/qbearerengine_p.h \
               bearer/qbearerplugin.h

    SOURCES += bearer/qnetworkconfigmanager_p.cpp \
               bearer/qbearerengine.cpp \
               bearer/qbearerplugin.cpp
}

