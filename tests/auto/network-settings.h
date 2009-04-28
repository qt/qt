/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QString>

#ifdef Q_OS_SYMBIAN
#include <sys/socket.h>
#include <net/if.h>
#endif
#if defined(Q_OS_SYMBIAN) && defined(Q_CC_NOKIAX86)
// In emulator we use WINSOCK connectivity by default. Unfortunately winsock
// does not work very well with UDP sockets. This defines skips some test
// cases which have known problems. 

// NOTE: Prefer to use WINPCAP based connectivity in S60 emulator when running
// network tests. WINPCAP connectivity uses Symbian OS IP stack, 
// correspondingly as HW does. When using WINPCAP disable this define
//#define SYMBIAN_WINSOCK_CONNECTIVITY
#endif

class QtNetworkSettings
{
public:
    static QString serverLocalName()
    {
        return QString("qttest");
        //return QString("aspiriniks");
        //return QString("qt-test-server");
    }
    static QString serverDomainName()
    {
        return QString("it.local");
        //return QString("troll.no");
    }
    static QString serverName()
    {
        return serverLocalName() + "." + serverDomainName();
    }
    static QString winServerName()
    {
        return serverName();
    }
    static QString wildcardServerName()
    {
        //return "qt-test-server.wildcard.dev." + serverDomainName();
        return "qttest.wildcard.dev." + serverDomainName();
    }
    static const char *serverIP()
    {
        //return "10.10.0.147";
        return "10.10.14.172";
        //return "10.3.7.2";
        //return "10.3.3.69";
    }
    
    static QByteArray expectedReplyIMAP()
    {
        //QByteArray expected( "* OK esparsett Cyrus IMAP4 v2.2.8 server ready\r\n" );
        
        
    	QByteArray expected( "* OK [CAPABILITY IMAP4 IMAP4rev1 LITERAL+ ID STARTTLS LOGINDISABLED] " );
  	    expected = expected.append(QtNetworkSettings::serverLocalName().toAscii());
        expected = expected.append(" Cyrus IMAP4 v2.3.11-Mandriva-RPM-2.3.11-6mdv2008.1 server ready\r\n");

        /*
        QByteArray expected( "* OK [CAPABILITY IMAP4 IMAP4REV1] " );
        expected = expected.append(QtNetworkSettings::serverLocalName().toAscii());
        expected = expected.append(" Cyrus IMAP4 v2.3.11-Mandriva-RPM-2.3.11-6mdv2008.1 server ready\r\n");
        */
        return expected;
    }

    static QByteArray expectedReplySSL()
    {
        QByteArray expected( "* OK [CAPABILITY IMAP4 IMAP4rev1 LITERAL+ ID AUTH=PLAIN SASL-IR] " );
        expected = expected.append(QtNetworkSettings::serverLocalName().toAscii());
        expected = expected.append(" Cyrus IMAP4 v2.3.11-Mandriva-RPM-2.3.11-6mdv2008.1 server ready\r\n");
        return expected;
    }
    
    static QByteArray expectedReplyFtp()
    {
        QByteArray expected( "220 (vsFTPd 2.0.5)\r\n221 Goodbye.\r\n" );
        return expected;
    }    

#ifdef Q_OS_SYMBIAN
    static void setDefaultIap()
    {
        struct ifreq ifReq;
        strcpy( ifReq.ifr_name, getDefaultIap().toAscii().constData());
        int err = setdefaultif( &ifReq );
        if(err)
            printf("Setting default IAP - '%s' failed: %d\n", getDefaultIap().toAscii().constData(), err);
        else
            printf("'%s' used as an default IAP\n", getDefaultIap().toAscii().constData());
    }
#endif

private:

#ifdef Q_OS_SYMBIAN
    static bool bIsIapLoaded;
    static QString iapFileFullPath;
    static QString defaultIap;

    static QString getDefaultIap() {
        if(!bIsIapLoaded)
            loadIapCfg();

        return defaultIap;
    }    

    static void loadIapCfg() {
        QFile iapCfgFile(iapFileFullPath);
        if (iapCfgFile.open(QFile::ReadOnly)) {
            QTextStream input(&iapCfgFile);
            QString line;
            bool bFoundDefaultTag = false;
            do {
                line = input.readLine().trimmed();
                if(line.startsWith(QString("#")))
                    continue; // comment found

                if(line.contains(QString("[DEFAULT]"))) {
                    bFoundDefaultTag = true;
                } else if(line.startsWith(QString("[")) && bFoundDefaultTag) {
                    break; // stick to default
                }

                if(bFoundDefaultTag && line.contains("name")) {
                    int position = line.indexOf(QString("="));
                    position += QString("=").length();
                    defaultIap = line.mid(position).trimmed();
                    break;
                }
            } while (!line.isNull());
        }
    }
#endif

};
#ifdef Q_OS_SYMBIAN
bool QtNetworkSettings::bIsIapLoaded = false;
QString QtNetworkSettings::iapFileFullPath = QString("C:\\Data\\iap.txt");
QString QtNetworkSettings::defaultIap = QString("Lab"); // this will be default value
#endif

#ifdef Q_OS_SYMBIAN
#define Q_SET_DEFAULT_IAP QtNetworkSettings::setDefaultIap();
#else
#define Q_SET_DEFAULT_IAP
#endif
