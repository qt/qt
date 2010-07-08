# Qt network module

TARGET   = QtNetwork
QPRO_PWD = $$PWD
DEFINES += QT_BUILD_NETWORK_LIB QT_NO_USING_NAMESPACE
#DEFINES += QLOCALSERVER_DEBUG QLOCALSOCKET_DEBUG
#DEFINES += QNETWORKDISKCACHE_DEBUG
#DEFINES += QSSLSOCKET_DEBUG
#DEFINES += QHOSTINFO_DEBUG
#DEFINES += QABSTRACTSOCKET_DEBUG QNATIVESOCKETENGINE_DEBUG
#DEFINES += QTCPSOCKETENGINE_DEBUG QTCPSOCKET_DEBUG QTCPSERVER_DEBUG QSSLSOCKET_DEBUG
#DEFINES += QUDPSOCKET_DEBUG QUDPSERVER_DEBUG
QT = core
win32-msvc*|win32-icc:QMAKE_LFLAGS += /BASE:0x64000000

unix:QMAKE_PKGCONFIG_REQUIRES = QtCore

include(../qbase.pri)
include(access/access.pri)
include(bearer/bearer.pri)
include(kernel/kernel.pri)
include(socket/socket.pri)
include(ssl/ssl.pri)

QMAKE_LIBS += $$QMAKE_LIBS_NETWORK


symbian {
   TARGET.UID3=0x2001B2DE
   LIBS += -lesock -linsock -lcertstore -lefsrv -lctframework

    # Partial upgrade SIS file
    vendorinfo = \
        "; Localised Vendor name" \
        "%{\"Nokia, Qt\"}" \
        " " \
        "; Unique Vendor name" \
        ":\"Nokia, Qt\"" \
        " "
    pu_header = "; Partial upgrade package for testing QtGui changes without reinstalling everything" \
                "$${LITERAL_HASH}{\"Qt network\"}, (0x2001E61C), $${QT_MAJOR_VERSION},$${QT_MINOR_VERSION},$${QT_PATCH_VERSION}, TYPE=PU"
    partial_upgrade.pkg_prerules = pu_header vendorinfo
    partial_upgrade.sources = $$QMAKE_LIBDIR_QT/QtNetwork.dll
    partial_upgrade.path = c:/sys/bin
    DEPLOYMENT = partial_upgrade $$DEPLOYMENT
}
