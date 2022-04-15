/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/
/*  This file is part of the KDE project
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License, or (at your option) version 3.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <QtTest/QtTest>
#include <QtCore/QDate>
#include <QtCore/QDebug>
#include <QtCore/QObject>
#include <QtCore/QUrl>

#ifndef Q_WS_WIN
#include <unistd.h>
#endif

#ifdef Q_OS_WINCE
#define MEDIA_FILE "/sax.wav"
#define MEDIA_FILEPATH ":/media/sax.wav"
const qint64 SEEK_BACKWARDS = 2000;
const qint64 ALLOWED_TIME_FOR_SEEKING = 1500; // 1.5s
const qint64 SEEKING_TOLERANCE = 250;
#else 
#if defined(Q_OS_WIN) || defined(Q_OS_MAC) || defined(Q_OS_SYMBIAN)
#define MEDIA_FILE "/sax.mp3"
#define MEDIA_FILEPATH ":/media/sax.mp3"
#else
#define MEDIA_FILE "/sax.ogg"
#define MEDIA_FILEPATH ":/media/sax.ogg"
#endif
const qint64 SEEK_BACKWARDS = 4000;
const qint64 ALLOWED_TIME_FOR_SEEKING = 1000; // 1s
const qint64 SEEKING_TOLERANCE = 0;
#endif //Q_OS_WINCE

#ifdef Q_OS_SYMBIAN
#include <cdbcols.h>
#include <cdblen.h>
#include <commdb.h>
#include <rconnmon.h>

const QString KDefaultIAP = QLatin1String("default");
const QString KInvalidIAP = QLatin1String("invalid IAP");

class CConnectionObserver : public CBase, public MConnectionMonitorObserver
{
public:
    static CConnectionObserver* NewL()
    {
        CConnectionObserver* self = new (ELeave) CConnectionObserver();
        CleanupStack::PushL(self);
        self->ConstructL();
        CleanupStack::Pop(self);
        return self;
    }
    QString currentIAP()
    {
        return m_currentIAPName;
    }
    ~CConnectionObserver()
    {
        m_connMon.Close();
    }
private:
    CConnectionObserver()
    {
    }
    void ConstructL()
    {
        m_connMon.ConnectL();
        m_connMon.NotifyEventL(*this);
    }
    void EventL (const CConnMonEventBase &aConnEvent)
    {
        TInt event = aConnEvent.EventType();
        TUint connId = aConnEvent.ConnectionId();
        TRequestStatus status;
        switch (event) {
        case EConnMonCreateConnection: {
                TBuf<KCommsDbSvrMaxColumnNameLength> iapName;
                m_connMon.GetStringAttribute(connId, 0, KIAPName, iapName, status);
                User::WaitForRequest(status);
                m_currentIAPName = QString(reinterpret_cast<const QChar *>(iapName.Ptr()), iapName.Length());
                qDebug() << "A new connection created using: " << m_currentIAPName;
                break;
        }
        default:
            break;
        }
    }

private:
    RConnectionMonitor m_connMon;
    QString m_currentIAPName;
};

#endif


class tst_MediaObject : public QObject
{
    Q_OBJECT
    public:
        tst_MediaObject()
            : m_success(false)
    {
        qputenv("PHONON_GST_AUDIOSINK", "fake");
    }

        bool m_success;
};


QTEST_MAIN(tst_MediaObject)

#include "tst_mediaobject.moc"
// vim: sw=4 ts=4
