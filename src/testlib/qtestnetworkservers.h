/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#ifndef QT_TESTNETWORKSERVERS_H
#define QT_TESTNETWORKSERVERS_H

#if defined(Q_OS_SYMBIAN) && !defined(Q_CC_NOKIAX86)
#define FTP_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define FTP_TEST_SERVER_IP_1 "10.215.7.68"
#define FTPPROXY_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define PROXY_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define SOCKS5PROXY_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define HTTPPROXY_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define HTTP_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define HTTPS_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define NTLM_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define UDP_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define TCP_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define SSH_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define ECHO_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define DAYTIME_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define WILDCARD_TEST_SERVER_NAME_1 "netiks.wildcard.dev.troll.no"
#define WILDCARD_TEST_SERVER_NAME_2 "netiks.*.troll.no"
#define DOMAIN_TEST_SERVER_NAME_1 ".troll.no"
#define DOMAIN_TEST_SERVER_NAME_2 "troll.no"
#define INTERN_TEST_SERVER_NAME_1 "intern.nokia.troll.no"
#define HOSTLOOKUP_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define HOSTLOOKUP_TEST_SERVER_IP "10.215.7.68"
#else
#define FTP_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define FTP_TEST_SERVER_IP_1 "10.215.7.68"
#define FTPPROXY_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define PROXY_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define SOCKS5PROXY_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define HTTPPROXY_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define HTTP_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define HTTPS_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define NTLM_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define UDP_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define TCP_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define SSH_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define ECHO_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define DAYTIME_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define WILDCARD_TEST_SERVER_NAME_1 "aspiriniks.wildcard.dev.europe.nokia.com"
#define WILDCARD_TEST_SERVER_NAME_2 "netiks.*.troll.no"
#define INTERN_TEST_SERVER_NAME_1 "intern.nokia.troll.no"
#define DOMAIN_TEST_SERVER_NAME_1 ".europe.nokia.com"
#define DOMAIN_TEST_SERVER_NAME_2 "europe.nokia.com"
#define HOSTLOOKUP_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define HOSTLOOKUP_TEST_SERVER_IP "10.215.7.68"
#endif

#define FTPS_TEST_SERVER_NAME_1 "aspiriniks.troll.no"
#define FOO_TEST_SERVER_NAME_1 HOSTWILLNEVEREXIST_TEST_SERVER_NAME_1
#define LOCALHOST_TEST_SERVER_NAME_1 "localhost"
#define HTTP_TEST_SERVER_NAME_4 "www.ietf.org"
#define NONEXISTING_TEST_SERVER_NAME_1 "this-host-will-never-exist.troll.no"
#define NONEXISTING_TEST_SERVER_NAME_2 "fluke-nosuchhost.troll.no"
#define NOSUCH_TEST_SERVER_NAME_1 "nosuchserver.troll.no"
#define IMAP_TEST_SERVER_NAME_1 "fluke.troll.no"
#define IMAP_TEST_SERVER_NAME_2 "imap.troll.no"
#define IMAP_TEST_SERVER_NAME_3 "imap.trolltech.com"
#define IMAPS_TEST_SERVER_NAME_1 "fluke.troll.no"
#define HOSTNOTFOUND_TEST_SERVER_NAME_1 "hostnotfoundhostnotfound.troll.no"
#define TIMEOUT_TEST_SERVER_NAME_1 "cisco.com"
#define UMA_TEST_SERVER_NAME_1 "fluke.troll.no"
#define HOSTWILLNEVEREXIST_TEST_SERVER_NAME_1 "this-host-will-never-exist.troll.no"

#define QBYTEARRAY_DEBUG(ba, maxsize)\
		printf( "ByteArray content (%d/%d bytes):\n", ba.size() <= maxsize ? ba.size() : maxsize , ba.size() ); \
    printf( "===========================================================================\n" ); \
    printf( ba.mid( 0, ba.size() <= maxsize ? ba.size() : maxsize ).constData() ); \
    printf( "\n==========================================================================\n" );
                                                  
                                                  
static const char *IFNAME = "Lab";

#endif // QT_TESTNETWORKSERVERS_H
