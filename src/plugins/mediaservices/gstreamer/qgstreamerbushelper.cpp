/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QMap>
#include <QTimer>
#include <QMutex>

#include "qgstreamerbushelper.h"


#ifndef QT_NO_GLIB
class QGstreamerBusHelperPrivate : public QObject
{
    Q_OBJECT

public:
    void addWatch(GstBus* bus, QGstreamerBusHelper* helper)
    {
        setParent(helper);
        m_tag = gst_bus_add_watch_full(bus, 0, busCallback, this, NULL);
        m_helper = helper;
        filter = 0;
    }

    void removeWatch(QGstreamerBusHelper* helper)
    {
        Q_UNUSED(helper);
        g_source_remove(m_tag);
    }

    static QGstreamerBusHelperPrivate* instance()
    {
        return new QGstreamerBusHelperPrivate;
    }

private:
    void processMessage(GstBus* bus, GstMessage* message)
    {
        Q_UNUSED(bus);
        emit m_helper->message(message);
    }

    static gboolean busCallback(GstBus *bus, GstMessage *message, gpointer data)
    {
        reinterpret_cast<QGstreamerBusHelperPrivate*>(data)->processMessage(bus, message);
        return TRUE;
    }

    guint       m_tag;
    QGstreamerBusHelper*  m_helper;

public:
    GstBus* bus;
    QGstreamerSyncEventFilter *filter;
    QMutex filterMutex;
};

#else

class QGstreamerBusHelperPrivate : public QObject
{
    Q_OBJECT
    typedef QMap<QGstreamerBusHelper*, GstBus*>   HelperMap;

public:
    void addWatch(GstBus* bus, QGstreamerBusHelper* helper)
    {
        m_helperMap.insert(helper, bus);

        if (m_helperMap.size() == 1)
            m_intervalTimer->start();
    }

    void removeWatch(QGstreamerBusHelper* helper)
    {
        m_helperMap.remove(helper);

        if (m_helperMap.size() == 0)
            m_intervalTimer->stop();
    }

    static QGstreamerBusHelperPrivate* instance()
    {
        static QGstreamerBusHelperPrivate self;

        return &self;
    }

private slots:
    void interval()
    {
        for (HelperMap::iterator it = m_helperMap.begin(); it != m_helperMap.end(); ++it) {
            GstMessage* message;

            while ((message = gst_bus_poll(it.value(), GST_MESSAGE_ANY, 0)) != 0) {
                emit it.key()->message(message);
                gst_message_unref(message);
            }

            emit it.key()->message(QGstreamerMessage());
        }
    }

private:
    QGstreamerBusHelperPrivate()
    {
        m_intervalTimer = new QTimer(this);
        m_intervalTimer->setInterval(250);

        connect(m_intervalTimer, SIGNAL(timeout()), SLOT(interval()));
    }

    HelperMap   m_helperMap;
    QTimer*     m_intervalTimer;

public:
    GstBus* bus;
    QGstreamerSyncEventFilter *filter;
    QMutex filterMutex;
};
#endif


static GstBusSyncReply syncGstBusFilter(GstBus* bus, GstMessage* message, QGstreamerBusHelperPrivate *d)
{
    Q_UNUSED(bus);
    QMutexLocker lock(&d->filterMutex);

    bool res = false;

    if (d->filter)
        res = d->filter->processSyncMessage(QGstreamerMessage(message));

    return res ? GST_BUS_DROP : GST_BUS_PASS;
}


/*!
    \class gstreamer::QGstreamerBusHelper
    \internal
*/

QGstreamerBusHelper::QGstreamerBusHelper(GstBus* bus, QObject* parent):
    QObject(parent),
    d(QGstreamerBusHelperPrivate::instance())
{
    d->bus = bus;
    d->addWatch(bus, this);

    gst_bus_set_sync_handler(bus, (GstBusSyncHandler)syncGstBusFilter, d);
}

QGstreamerBusHelper::~QGstreamerBusHelper()
{
    d->removeWatch(this);
    gst_bus_set_sync_handler(d->bus,0,0);
}

void QGstreamerBusHelper::installSyncEventFilter(QGstreamerSyncEventFilter *filter)
{
    QMutexLocker lock(&d->filterMutex);
    d->filter = filter;
}

#include "qgstreamerbushelper.moc"
