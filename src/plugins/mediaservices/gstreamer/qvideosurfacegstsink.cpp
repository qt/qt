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

#include <QtMultimedia/QAbstractVideoSurface>
#include <QtMultimedia/QVideoFrame>
#include <QtCore/qdebug.h>
#include <QMap>
#include <QThread>

#include "qgstvideobuffer.h"

#ifdef Q_WS_X11
#include <QtGui/qx11info_x11.h>
#include "qgstxvimagebuffer.h"
#endif

#include "qvideosurfacegstsink.h"




Q_DECLARE_METATYPE(QVideoSurfaceFormat)

QT_BEGIN_NAMESPACE

QVideoSurfaceGstDelegate::QVideoSurfaceGstDelegate(QAbstractVideoSurface *surface)
    : m_surface(surface)
    , m_renderReturn(GST_FLOW_ERROR)
    , m_bytesPerLine(0)
{
    m_supportedPixelFormats = m_surface->supportedPixelFormats();

    connect(m_surface, SIGNAL(supportedFormatsChanged()), this, SLOT(supportedFormatsChanged()));
}

QList<QVideoFrame::PixelFormat> QVideoSurfaceGstDelegate::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    QMutexLocker locker(const_cast<QMutex *>(&m_mutex));

    if (handleType == QAbstractVideoBuffer::NoHandle)
        return m_supportedPixelFormats;
    else
        return m_surface->supportedPixelFormats(handleType);
}

QVideoSurfaceFormat QVideoSurfaceGstDelegate::surfaceFormat() const
{
    QMutexLocker locker(const_cast<QMutex *>(&m_mutex));
    return m_format;
}

bool QVideoSurfaceGstDelegate::start(const QVideoSurfaceFormat &format, int bytesPerLine)
{
    QMutexLocker locker(&m_mutex);

    m_format = format;
    m_bytesPerLine = bytesPerLine;

    if (QThread::currentThread() == thread()) {
        m_started = !m_surface.isNull() ? m_surface->start(m_format) : false;
    } else {
        QMetaObject::invokeMethod(this, "queuedStart", Qt::QueuedConnection);

        m_setupCondition.wait(&m_mutex);
    }

    m_format = m_surface->surfaceFormat();

    return m_started;
}

void QVideoSurfaceGstDelegate::stop()
{
    QMutexLocker locker(&m_mutex);

    if (QThread::currentThread() == thread()) {
        if (!m_surface.isNull())
            m_surface->stop();
    } else {
        QMetaObject::invokeMethod(this, "queuedStop", Qt::QueuedConnection);

        m_setupCondition.wait(&m_mutex);
    }

    m_started = false;
}

bool QVideoSurfaceGstDelegate::isActive()
{
    QMutexLocker locker(&m_mutex);
    return m_surface->isActive();
}

GstFlowReturn QVideoSurfaceGstDelegate::render(GstBuffer *buffer)
{
    QMutexLocker locker(&m_mutex);

    QGstVideoBuffer *videoBuffer = 0;

#ifdef Q_WS_X11
    if (G_TYPE_CHECK_INSTANCE_TYPE(buffer, QGstXvImageBuffer::get_type())) {
        QGstXvImageBuffer *xvBuffer = reinterpret_cast<QGstXvImageBuffer *>(buffer);
        QVariant handle = QVariant::fromValue(xvBuffer->xvImage);
        videoBuffer = new QGstVideoBuffer(buffer, m_bytesPerLine, XvHandleType, handle);
    } else
#endif
        videoBuffer = new QGstVideoBuffer(buffer, m_bytesPerLine);

    m_frame = QVideoFrame(
            videoBuffer,
            m_format.frameSize(),
            m_format.pixelFormat());

    qint64 startTime = GST_BUFFER_TIMESTAMP(buffer);

    if (startTime >= 0) {
        m_frame.setStartTime(startTime/G_GINT64_CONSTANT (1000000));

        qint64 duration = GST_BUFFER_DURATION(buffer);

        if (duration >= 0)
            m_frame.setEndTime((startTime + duration)/G_GINT64_CONSTANT (1000000));
    }

    QMetaObject::invokeMethod(this, "queuedRender", Qt::QueuedConnection);

    if (!m_renderCondition.wait(&m_mutex, 300)) {
        m_frame = QVideoFrame();

        return GST_FLOW_OK;
    } else {
        return m_renderReturn;
    }
}

void QVideoSurfaceGstDelegate::queuedStart()
{
    QMutexLocker locker(&m_mutex);

    m_started = m_surface->start(m_format);

    m_setupCondition.wakeAll();
}

void QVideoSurfaceGstDelegate::queuedStop()
{
    QMutexLocker locker(&m_mutex);

    m_surface->stop();

    m_setupCondition.wakeAll();
}

void QVideoSurfaceGstDelegate::queuedRender()
{
    QMutexLocker locker(&m_mutex);

    if (m_surface.isNull()) {
        m_renderReturn = GST_FLOW_ERROR;
    } else if (m_surface->present(m_frame)) {
        m_renderReturn = GST_FLOW_OK;
    } else {
        switch (m_surface->error()) {
        case QAbstractVideoSurface::NoError:
            m_renderReturn = GST_FLOW_OK;
            break;
        case QAbstractVideoSurface::StoppedError:
            m_renderReturn = GST_FLOW_NOT_NEGOTIATED;
            break;
        default:
            m_renderReturn = GST_FLOW_ERROR;
            break;
        }
    }

    m_renderCondition.wakeAll();
}

void QVideoSurfaceGstDelegate::supportedFormatsChanged()
{
    QMutexLocker locker(&m_mutex);

    m_supportedPixelFormats = m_surface->supportedPixelFormats();
}

struct YuvFormat
{
    QVideoFrame::PixelFormat pixelFormat;
    guint32 fourcc;
    int bitsPerPixel;
};

static const YuvFormat qt_yuvColorLookup[] =
{
    { QVideoFrame::Format_YUV420P, GST_MAKE_FOURCC('I','4','2','0'), 8 },
    { QVideoFrame::Format_YV12,    GST_MAKE_FOURCC('Y','V','1','2'), 8 },
    { QVideoFrame::Format_UYVY,    GST_MAKE_FOURCC('U','Y','V','Y'), 16 },
    { QVideoFrame::Format_YUYV,    GST_MAKE_FOURCC('Y','U','Y','2'), 16 },
    { QVideoFrame::Format_NV12,    GST_MAKE_FOURCC('N','V','1','2'), 8 },
    { QVideoFrame::Format_NV21,    GST_MAKE_FOURCC('N','V','2','1'), 8 },
    { QVideoFrame::Format_AYUV444, GST_MAKE_FOURCC('A','Y','U','V'), 32 }
};

static int indexOfYuvColor(QVideoFrame::PixelFormat format)
{
    const int count = sizeof(qt_yuvColorLookup) / sizeof(YuvFormat);

    for (int i = 0; i < count; ++i)
        if (qt_yuvColorLookup[i].pixelFormat == format)
            return i;

    return -1;
}

static int indexOfYuvColor(guint32 fourcc)
{
    const int count = sizeof(qt_yuvColorLookup) / sizeof(YuvFormat);

    for (int i = 0; i < count; ++i)
        if (qt_yuvColorLookup[i].fourcc == fourcc)
            return i;

    return -1;
}

struct RgbFormat
{
    QVideoFrame::PixelFormat pixelFormat;
    int bitsPerPixel;
    int depth;
    int endianness;
    int red;
    int green;
    int blue;
    int alpha;
};

static const RgbFormat qt_rgbColorLookup[] =
{
    { QVideoFrame::Format_RGB32 , 32, 24, 4321, 0x0000FF00, 0x00FF0000, 0xFF000000, 0x00000000 },
    { QVideoFrame::Format_RGB32 , 32, 24, 1234, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    { QVideoFrame::Format_BGR32 , 32, 24, 4321, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x00000000 },
    { QVideoFrame::Format_BGR32 , 32, 24, 1234, 0x000000FF, 0x0000FF00, 0x00FF0000, 0x00000000 },
    { QVideoFrame::Format_ARGB32, 32, 24, 4321, 0x0000FF00, 0x00FF0000, 0xFF000000, 0x000000FF },
    { QVideoFrame::Format_ARGB32, 32, 24, 1234, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000 },
    { QVideoFrame::Format_RGB24 , 24, 24, 4321, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    { QVideoFrame::Format_BGR24 , 24, 24, 4321, 0x000000FF, 0x0000FF00, 0x00FF0000, 0x00000000 },
    { QVideoFrame::Format_RGB565, 16, 16, 1234, 0x0000F800, 0x000007E0, 0x0000001F, 0x00000000 }
};

static int indexOfRgbColor(
        int bits, int depth, int endianness, int red, int green, int blue, int alpha)
{
    const int count = sizeof(qt_rgbColorLookup) / sizeof(RgbFormat);

    for (int i = 0; i < count; ++i) {
        if (qt_rgbColorLookup[i].bitsPerPixel == bits
            && qt_rgbColorLookup[i].depth == depth
            && qt_rgbColorLookup[i].endianness == endianness
            && qt_rgbColorLookup[i].red == red
            && qt_rgbColorLookup[i].green == green
            && qt_rgbColorLookup[i].blue == blue
            && qt_rgbColorLookup[i].alpha == alpha) {
            return i;
        }
    }
    return -1;
}

static GstVideoSinkClass *sink_parent_class;

#define VO_SINK(s) QVideoSurfaceGstSink *sink(reinterpret_cast<QVideoSurfaceGstSink *>(s))

QVideoSurfaceGstSink *QVideoSurfaceGstSink::createSink(QAbstractVideoSurface *surface)
{
    QVideoSurfaceGstSink *sink = reinterpret_cast<QVideoSurfaceGstSink *>(
            g_object_new(QVideoSurfaceGstSink::get_type(), 0));

    sink->delegate = new QVideoSurfaceGstDelegate(surface);

    return sink;
}

GType QVideoSurfaceGstSink::get_type()
{
    static GType type = 0;

    if (type == 0) {
        static const GTypeInfo info =
        {
            sizeof(QVideoSurfaceGstSinkClass),                    // class_size
            base_init,                                         // base_init
            NULL,                                              // base_finalize
            class_init,                                        // class_init
            NULL,                                              // class_finalize
            NULL,                                              // class_data
            sizeof(QVideoSurfaceGstSink),                         // instance_size
            0,                                                 // n_preallocs
            instance_init,                                     // instance_init
            0                                                  // value_table
        };

        type = g_type_register_static(
                GST_TYPE_VIDEO_SINK, "QVideoSurfaceGstSink", &info, GTypeFlags(0));
    }

    return type;
}

void QVideoSurfaceGstSink::class_init(gpointer g_class, gpointer class_data)
{
    Q_UNUSED(class_data);

    sink_parent_class = reinterpret_cast<GstVideoSinkClass *>(g_type_class_peek_parent(g_class));

    GstBaseSinkClass *base_sink_class = reinterpret_cast<GstBaseSinkClass *>(g_class);
    base_sink_class->get_caps = QVideoSurfaceGstSink::get_caps;
    base_sink_class->set_caps = QVideoSurfaceGstSink::set_caps;
    base_sink_class->buffer_alloc = QVideoSurfaceGstSink::buffer_alloc;
    base_sink_class->start = QVideoSurfaceGstSink::start;
    base_sink_class->stop = QVideoSurfaceGstSink::stop;
    // base_sink_class->unlock = QVideoSurfaceGstSink::unlock; // Not implemented.
    // base_sink_class->event = QVideoSurfaceGstSink::event; // Not implemented.
    base_sink_class->preroll = QVideoSurfaceGstSink::preroll;
    base_sink_class->render = QVideoSurfaceGstSink::render;

    GstElementClass *element_class = reinterpret_cast<GstElementClass *>(g_class);
    element_class->change_state = QVideoSurfaceGstSink::change_state;

    GObjectClass *object_class = reinterpret_cast<GObjectClass *>(g_class);
    object_class->finalize = QVideoSurfaceGstSink::finalize;
}

void QVideoSurfaceGstSink::base_init(gpointer g_class)
{
    static GstStaticPadTemplate sink_pad_template = GST_STATIC_PAD_TEMPLATE(
            "sink", GST_PAD_SINK, GST_PAD_ALWAYS, GST_STATIC_CAPS(
                    "video/x-raw-rgb, "
                    "framerate = (fraction) [ 0, MAX ], "
                    "width = (int) [ 1, MAX ], "
                    "height = (int) [ 1, MAX ]; "
                    "video/x-raw-yuv, "
                    "framerate = (fraction) [ 0, MAX ], "
                    "width = (int) [ 1, MAX ], "
                    "height = (int) [ 1, MAX ]"));

    gst_element_class_add_pad_template(
            GST_ELEMENT_CLASS(g_class), gst_static_pad_template_get(&sink_pad_template));
}

void QVideoSurfaceGstSink::instance_init(GTypeInstance *instance, gpointer g_class)
{
    VO_SINK(instance);

    Q_UNUSED(g_class);

    sink->delegate = 0;
#ifdef Q_WS_X11
    sink->pool = new QGstXvImageBufferPool();
#endif
    sink->lastRequestedCaps = 0;
    sink->lastBufferCaps = 0;
    sink->lastSurfaceFormat = new QVideoSurfaceFormat;
}

void QVideoSurfaceGstSink::finalize(GObject *object)
{
    VO_SINK(object);
#ifdef Q_WS_X11
    delete sink->pool;
    sink->pool = 0;
#endif

    delete sink->lastSurfaceFormat;
    sink->lastSurfaceFormat = 0;

    if (sink->lastBufferCaps)
        gst_caps_unref(sink->lastBufferCaps);
    sink->lastBufferCaps = 0;

    if (sink->lastRequestedCaps)
        gst_caps_unref(sink->lastRequestedCaps);
    sink->lastRequestedCaps = 0;
}

GstStateChangeReturn QVideoSurfaceGstSink::change_state(
        GstElement *element, GstStateChange transition)
{
    Q_UNUSED(element);

    return GST_ELEMENT_CLASS(sink_parent_class)->change_state(
            element, transition);
}

GstCaps *QVideoSurfaceGstSink::get_caps(GstBaseSink *base)
{
    VO_SINK(base);

    GstCaps *caps = gst_caps_new_empty();

    foreach (QVideoFrame::PixelFormat format, sink->delegate->supportedPixelFormats()) {
        int index = indexOfYuvColor(format);

        if (index != -1) {
            gst_caps_append_structure(caps, gst_structure_new(
                    "video/x-raw-yuv",
                    "framerate", GST_TYPE_FRACTION_RANGE, 0, 1, INT_MAX, 1,
                    "width"    , GST_TYPE_INT_RANGE, 1, INT_MAX,
                    "height"   , GST_TYPE_INT_RANGE, 1, INT_MAX,
                    "format"   , GST_TYPE_FOURCC, qt_yuvColorLookup[index].fourcc,
                    NULL));
            continue;
        }

        const int count = sizeof(qt_rgbColorLookup) / sizeof(RgbFormat);

        for (int i = 0; i < count; ++i) {
            if (qt_rgbColorLookup[i].pixelFormat == format) {
                GstStructure *structure = gst_structure_new(
                        "video/x-raw-rgb",
                        "framerate" , GST_TYPE_FRACTION_RANGE, 0, 1, INT_MAX, 1,
                        "width"     , GST_TYPE_INT_RANGE, 1, INT_MAX,
                        "height"    , GST_TYPE_INT_RANGE, 1, INT_MAX,
                        "bpp"       , G_TYPE_INT, qt_rgbColorLookup[i].bitsPerPixel,
                        "depth"     , G_TYPE_INT, qt_rgbColorLookup[i].depth,
                        "endianness", G_TYPE_INT, qt_rgbColorLookup[i].endianness,
                        "red_mask"  , G_TYPE_INT, qt_rgbColorLookup[i].red,
                        "green_mask", G_TYPE_INT, qt_rgbColorLookup[i].green,
                        "blue_mask" , G_TYPE_INT, qt_rgbColorLookup[i].blue,
                        NULL);

                if (qt_rgbColorLookup[i].alpha != 0) {
                    gst_structure_set(
                            structure, "alpha_mask", G_TYPE_INT, qt_rgbColorLookup[i].alpha, NULL);
                }
                gst_caps_append_structure(caps, structure);
            }
        }
    }

    return caps;
}

gboolean QVideoSurfaceGstSink::set_caps(GstBaseSink *base, GstCaps *caps)
{
    VO_SINK(base);

    //qDebug() << "set_caps";
    //qDebug() << gst_caps_to_string(caps);

    if (!caps) {
        sink->delegate->stop();

        return TRUE;
    } else {
        int bytesPerLine = 0;
        QVideoSurfaceFormat format = formatForCaps(caps, &bytesPerLine);

        if (sink->delegate->isActive()) {
            QVideoSurfaceFormat surfaceFormst = sink->delegate->surfaceFormat();

            if (format.pixelFormat() == surfaceFormst.pixelFormat() &&
                format.frameSize() == surfaceFormst.frameSize())
                return TRUE;
            else
                sink->delegate->stop();
        }

        if (sink->lastRequestedCaps)
            gst_caps_unref(sink->lastRequestedCaps);
        sink->lastRequestedCaps = 0;

        //qDebug() << "Staring video surface:";
        //qDebug() << format;
        //qDebug() << bytesPerLine;

        if (sink->delegate->start(format, bytesPerLine))
            return TRUE;

    }

    return FALSE;
}

QVideoSurfaceFormat QVideoSurfaceGstSink::formatForCaps(GstCaps *caps, int *bytesPerLine)
{
    const GstStructure *structure = gst_caps_get_structure(caps, 0);

    QVideoFrame::PixelFormat pixelFormat = QVideoFrame::Format_Invalid;
    int bitsPerPixel = 0;

    QSize size;
    gst_structure_get_int(structure, "width", &size.rwidth());
    gst_structure_get_int(structure, "height", &size.rheight());

    if (qstrcmp(gst_structure_get_name(structure), "video/x-raw-yuv") == 0) {
        guint32 fourcc = 0;
        gst_structure_get_fourcc(structure, "format", &fourcc);

        int index = indexOfYuvColor(fourcc);
        if (index != -1) {
            pixelFormat = qt_yuvColorLookup[index].pixelFormat;
            bitsPerPixel = qt_yuvColorLookup[index].bitsPerPixel;
        }
    } else if (qstrcmp(gst_structure_get_name(structure), "video/x-raw-rgb") == 0) {
        int depth = 0;
        int endianness = 0;
        int red = 0;
        int green = 0;
        int blue = 0;
        int alpha = 0;

        gst_structure_get_int(structure, "bpp", &bitsPerPixel);
        gst_structure_get_int(structure, "depth", &depth);
        gst_structure_get_int(structure, "endianness", &endianness);
        gst_structure_get_int(structure, "red_mask", &red);
        gst_structure_get_int(structure, "green_mask", &green);
        gst_structure_get_int(structure, "blue_mask", &blue);
        gst_structure_get_int(structure, "alpha_mask", &alpha);

        int index = indexOfRgbColor(bitsPerPixel, depth, endianness, red, green, blue, alpha);

        if (index != -1)
            pixelFormat = qt_rgbColorLookup[index].pixelFormat;
    }

    if (pixelFormat != QVideoFrame::Format_Invalid) {
        QVideoSurfaceFormat format(size, pixelFormat);

        QPair<int, int> rate;
        gst_structure_get_fraction(structure, "framerate", &rate.first, &rate.second);

        if (rate.second)
            format.setFrameRate(qreal(rate.first)/rate.second);

        gint aspectNum = 0;
        gint aspectDenum = 0;
        if (gst_structure_get_fraction(
                structure, "pixel-aspect-ratio", &aspectNum, &aspectDenum)) {
            if (aspectDenum > 0)
                format.setPixelAspectRatio(aspectNum, aspectDenum);
        }

        if (bytesPerLine)
            *bytesPerLine = ((size.width() * bitsPerPixel / 8) + 3) & ~3;

        return format;
    }

    return QVideoSurfaceFormat();
}


GstFlowReturn QVideoSurfaceGstSink::buffer_alloc(
        GstBaseSink *base, guint64 offset, guint size, GstCaps *caps, GstBuffer **buffer)
{
    VO_SINK(base);

    Q_UNUSED(offset);
    Q_UNUSED(size);

    *buffer = 0;

#ifdef Q_WS_X11

    if (sink->lastRequestedCaps && gst_caps_is_equal(sink->lastRequestedCaps, caps)) {
        //qDebug() << "reusing last caps";
        *buffer = GST_BUFFER(sink->pool->takeBuffer(*sink->lastSurfaceFormat, sink->lastBufferCaps));
        return GST_FLOW_OK;
    }

    if (sink->delegate->supportedPixelFormats(XvHandleType).isEmpty()) {
        //qDebug() << "sink doesn't support Xv buffers, skip buffers allocation";
        return GST_FLOW_OK;
    }

    GstCaps *intersection = gst_caps_intersect(get_caps(GST_BASE_SINK(sink)), caps);

    if (gst_caps_is_empty (intersection)) {
        gst_caps_unref(intersection);
        return GST_FLOW_NOT_NEGOTIATED;
    }

    if (sink->delegate->isActive()) {
        //if format was changed, restart the surface
        QVideoSurfaceFormat format = formatForCaps(intersection);
        QVideoSurfaceFormat surfaceFormat = sink->delegate->surfaceFormat();

        if (format.pixelFormat() != surfaceFormat.pixelFormat() ||
            format.frameSize() != surfaceFormat.frameSize()) {
            //qDebug() << "new format requested, restart video surface";
            sink->delegate->stop();
        }
    }

    if (!sink->delegate->isActive()) {
        int bytesPerLine = 0;
        QVideoSurfaceFormat format = formatForCaps(intersection, &bytesPerLine);

        if (!sink->delegate->start(format, bytesPerLine)) {
            qDebug() << "failed to start video surface";
            return GST_FLOW_NOT_NEGOTIATED;
        }
    }

    QVideoSurfaceFormat surfaceFormat = sink->delegate->surfaceFormat();

    if (!sink->pool->isFormatSupported(surfaceFormat)) {
        //qDebug() << "sink doesn't provide Xv buffer details, skip buffers allocation";
        return GST_FLOW_OK;
    }

    if (sink->lastRequestedCaps)
        gst_caps_unref(sink->lastRequestedCaps);
    sink->lastRequestedCaps = caps;
    gst_caps_ref(sink->lastRequestedCaps);

    if (sink->lastBufferCaps)
        gst_caps_unref(sink->lastBufferCaps);
    sink->lastBufferCaps = intersection;
    gst_caps_ref(sink->lastBufferCaps);

    *sink->lastSurfaceFormat = surfaceFormat;

    *buffer =  GST_BUFFER(sink->pool->takeBuffer(surfaceFormat, intersection));

#endif
    return GST_FLOW_OK;
}

gboolean QVideoSurfaceGstSink::start(GstBaseSink *base)
{
    Q_UNUSED(base);

    return TRUE;
}

gboolean QVideoSurfaceGstSink::stop(GstBaseSink *base)
{
    Q_UNUSED(base);

    return TRUE;
}

gboolean QVideoSurfaceGstSink::unlock(GstBaseSink *base)
{
    Q_UNUSED(base);

    return TRUE;
}

gboolean QVideoSurfaceGstSink::event(GstBaseSink *base, GstEvent *event)
{
    Q_UNUSED(base);
    Q_UNUSED(event);

    return TRUE;
}

GstFlowReturn QVideoSurfaceGstSink::preroll(GstBaseSink *base, GstBuffer *buffer)
{
    VO_SINK(base);

    return sink->delegate->render(buffer);
}

GstFlowReturn QVideoSurfaceGstSink::render(GstBaseSink *base, GstBuffer *buffer)
{
    VO_SINK(base);
    return sink->delegate->render(buffer);
}

QT_END_NAMESPACE


