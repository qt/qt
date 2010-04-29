/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#ifndef VIDEOSURFACEGSTSINK_H
#define VIDEOSURFACEGSTSINK_H

#include <gst/video/gstvideosink.h>

#include <QtCore/qlist.h>
#include <QtCore/qmutex.h>
#include <QtCore/qpointer.h>
#include <QtCore/qwaitcondition.h>
#include <QtMultimedia/qvideosurfaceformat.h>
#include <QtMultimedia/qvideoframe.h>
#include <QtMultimedia/qabstractvideobuffer.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QAbstractVideoSurface;

#ifdef Q_WS_X11
class QGstXvImageBuffer;
class QGstXvImageBufferPool;
#endif


class QVideoSurfaceGstDelegate : public QObject
{
    Q_OBJECT
public:
    QVideoSurfaceGstDelegate(QAbstractVideoSurface *surface);

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
        QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;

    QVideoSurfaceFormat surfaceFormat() const;


    bool start(const QVideoSurfaceFormat &format, int bytesPerLine);
    void stop();

    bool isActive();

    GstFlowReturn render(GstBuffer *buffer);

private slots:
    void queuedStart();
    void queuedStop();
    void queuedRender();

    void supportedFormatsChanged();

private:
    QPointer<QAbstractVideoSurface> m_surface;
    QList<QVideoFrame::PixelFormat> m_supportedPixelFormats;
    QMutex m_mutex;
    QWaitCondition m_setupCondition;
    QWaitCondition m_renderCondition;
    QVideoSurfaceFormat m_format;
    QVideoFrame m_frame;
    GstFlowReturn m_renderReturn;
    int m_bytesPerLine;
    bool m_started;
};

class QVideoSurfaceGstSink
{
public:
    GstVideoSink parent;

    static QVideoSurfaceGstSink *createSink(QAbstractVideoSurface *surface);
    static QVideoSurfaceFormat formatForCaps(GstCaps *caps, int *bytesPerLine = 0);

private:
    static GType get_type();
    static void class_init(gpointer g_class, gpointer class_data);
    static void base_init(gpointer g_class);
    static void instance_init(GTypeInstance *instance, gpointer g_class);

    static void finalize(GObject *object);

    static GstStateChangeReturn change_state(GstElement *element, GstStateChange transition);

    static GstCaps *get_caps(GstBaseSink *sink);
    static gboolean set_caps(GstBaseSink *sink, GstCaps *caps);

    static GstFlowReturn buffer_alloc(
            GstBaseSink *sink, guint64 offset, guint size, GstCaps *caps, GstBuffer **buffer);

    static gboolean start(GstBaseSink *sink);
    static gboolean stop(GstBaseSink *sink);

    static gboolean unlock(GstBaseSink *sink);

    static gboolean event(GstBaseSink *sink, GstEvent *event);
    static GstFlowReturn preroll(GstBaseSink *sink, GstBuffer *buffer);
    static GstFlowReturn render(GstBaseSink *sink, GstBuffer *buffer);

private:
    QVideoSurfaceGstDelegate *delegate;

#ifdef Q_WS_X11
    QGstXvImageBufferPool *pool;
#endif

    GstCaps *lastRequestedCaps;
    GstCaps *lastBufferCaps;
    QVideoSurfaceFormat *lastSurfaceFormat;
};


class QVideoSurfaceGstSinkClass
{
public:
    GstVideoSinkClass parent_class;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
