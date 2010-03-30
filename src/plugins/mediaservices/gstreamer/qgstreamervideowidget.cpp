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

#include "qgstreamervideowidget.h"

#include <QtCore/qcoreevent.h>
#include <QtCore/qdebug.h>
#include <QtGui/qapplication.h>
#include <QtGui/qpainter.h>

#include <X11/Xlib.h>
#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>
#include <gst/interfaces/propertyprobe.h>


QT_BEGIN_NAMESPACE

class QGstreamerVideoWidget : public QWidget
{
public:
    QGstreamerVideoWidget(QWidget *parent = 0)
        :QWidget(parent)
    {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        QPalette palette;
        palette.setColor(QPalette::Background, Qt::black);
        setPalette(palette);
    }

    virtual ~QGstreamerVideoWidget() {}

    QSize sizeHint() const
    {
        return m_nativeSize;
    }

    void setNativeSize( const QSize &size)
    {
        if (size != m_nativeSize) {
            m_nativeSize = size;
            if (size.isEmpty())
                setMinimumSize(0,0);
            else
                setMinimumSize(160,120);

            updateGeometry();
        }
    }

protected:
    void paintEvent(QPaintEvent *)
    {
        QPainter painter(this);
        painter.fillRect(rect(), palette().background());
    }

    QSize m_nativeSize;
};

QGstreamerVideoWidgetControl::QGstreamerVideoWidgetControl(QObject *parent)
    : QVideoWidgetControl(parent)
    , m_videoSink(0)
    , m_widget(0)
    , m_fullScreen(false)
{    
}

QGstreamerVideoWidgetControl::~QGstreamerVideoWidgetControl()
{
    if (m_videoSink)
        gst_object_unref(GST_OBJECT(m_videoSink));

    delete m_widget;
}

void QGstreamerVideoWidgetControl::createVideoWidget()
{
    if (m_widget)
        return;

    m_widget = new QGstreamerVideoWidget;

    m_widget->installEventFilter(this);
    m_windowId = m_widget->winId();

    m_videoSink = gst_element_factory_make ("xvimagesink", NULL);
    if (m_videoSink) {
        // Check if the xv sink is usable
        if (gst_element_set_state(m_videoSink, GST_STATE_READY) != GST_STATE_CHANGE_SUCCESS) {
            gst_object_unref(GST_OBJECT(m_videoSink));
            m_videoSink = 0;
        } else {
            gst_element_set_state(m_videoSink, GST_STATE_NULL);

            g_object_set(G_OBJECT(m_videoSink), "force-aspect-ratio", 1, (const char*)NULL);
        }
    }

    if (!m_videoSink)
        m_videoSink = gst_element_factory_make ("ximagesink", NULL);

    gst_object_ref (GST_OBJECT (m_videoSink)); //Take ownership
    gst_object_sink (GST_OBJECT (m_videoSink));
}

GstElement *QGstreamerVideoWidgetControl::videoSink()
{
    createVideoWidget();
    return m_videoSink;
}

bool QGstreamerVideoWidgetControl::eventFilter(QObject *object, QEvent *e)
{
    if (m_widget && object == m_widget) {
        if (e->type() == QEvent::ParentChange || e->type() == QEvent::Show) {
            WId newWId = m_widget->winId();
            if (newWId != m_windowId) {
                m_windowId = newWId;
                // Even if we have created a winId at this point, other X applications
                // need to be aware of it.
                QApplication::syncX();
                setOverlay();
            }
        }

        if (e->type() == QEvent::Show) {
            // Setting these values ensures smooth resizing since it
            // will prevent the system from clearing the background
            m_widget->setAttribute(Qt::WA_NoSystemBackground, true);
            m_widget->setAttribute(Qt::WA_PaintOnScreen, true);
        } else if (e->type() == QEvent::Resize) {
            // This is a workaround for missing background repaints
            // when reducing window size
            windowExposed();
        }
    }

    return false;
}

void QGstreamerVideoWidgetControl::precessNewStream()
{
    setOverlay();
    QMetaObject::invokeMethod(this, "updateNativeVideoSize", Qt::QueuedConnection);
}

void QGstreamerVideoWidgetControl::setOverlay()
{
    if (m_videoSink && GST_IS_X_OVERLAY(m_videoSink)) {
        gst_x_overlay_set_xwindow_id(GST_X_OVERLAY(m_videoSink), m_windowId);
    }
}

void QGstreamerVideoWidgetControl::updateNativeVideoSize()
{
    if (m_videoSink) {
        //find video native size to update video widget size hint
        GstPad *pad = gst_element_get_static_pad(m_videoSink,"sink");
        GstCaps *caps = gst_pad_get_negotiated_caps(pad);

        if (caps) {
            GstStructure *str;
            gint width, height;

            if ((str = gst_caps_get_structure (caps, 0))) {
                if (gst_structure_get_int (str, "width", &width) && gst_structure_get_int (str, "height", &height)) {
                    gint aspectNum = 0;
                    gint aspectDenum = 0;
                    if (gst_structure_get_fraction(str, "pixel-aspect-ratio", &aspectNum, &aspectDenum)) {
                        if (aspectDenum > 0)
                            width = width*aspectNum/aspectDenum;
                    }
                    m_widget->setNativeSize(QSize(width, height));
                }
            }
            gst_caps_unref(caps);
        }
    } else {
        if (m_widget)
            m_widget->setNativeSize(QSize());
    }
}


void QGstreamerVideoWidgetControl::windowExposed()
{
    if (m_videoSink && GST_IS_X_OVERLAY(m_videoSink))
        gst_x_overlay_expose(GST_X_OVERLAY(m_videoSink));
}

QWidget *QGstreamerVideoWidgetControl::videoWidget()
{
    createVideoWidget();
    return m_widget;
}

Qt::AspectRatioMode QGstreamerVideoWidgetControl::aspectRatioMode() const
{
    return m_aspectRatioMode;
}

void QGstreamerVideoWidgetControl::setAspectRatioMode(Qt::AspectRatioMode mode)
{
    if (m_videoSink) {
        g_object_set(G_OBJECT(m_videoSink),
                     "force-aspect-ratio",
                     (mode == Qt::KeepAspectRatio),
                     (const char*)NULL);
    }

    m_aspectRatioMode = mode;
}

bool QGstreamerVideoWidgetControl::isFullScreen() const
{
    return m_fullScreen;
}

void QGstreamerVideoWidgetControl::setFullScreen(bool fullScreen)
{
    emit fullScreenChanged(m_fullScreen =  fullScreen);
}

int QGstreamerVideoWidgetControl::brightness() const
{
    int brightness = 0;

    if (m_videoSink && g_object_class_find_property(G_OBJECT_GET_CLASS(m_videoSink), "brightness"))
        g_object_get(G_OBJECT(m_videoSink), "brightness", &brightness, NULL);

    return brightness / 10;
}

void QGstreamerVideoWidgetControl::setBrightness(int brightness)
{
    if (m_videoSink && g_object_class_find_property(G_OBJECT_GET_CLASS(m_videoSink), "brightness")) {
        g_object_set(G_OBJECT(m_videoSink), "brightness", brightness * 10, NULL);

        emit brightnessChanged(brightness);
    }
}

int QGstreamerVideoWidgetControl::contrast() const
{
    int contrast = 0;

    if (m_videoSink && g_object_class_find_property(G_OBJECT_GET_CLASS(m_videoSink), "contrast"))
        g_object_get(G_OBJECT(m_videoSink), "contrast", &contrast, NULL);

    return contrast / 10;
}

void QGstreamerVideoWidgetControl::setContrast(int contrast)
{
    if (m_videoSink && g_object_class_find_property(G_OBJECT_GET_CLASS(m_videoSink), "contrast")) {
        g_object_set(G_OBJECT(m_videoSink), "contrast", contrast * 10, NULL);

        emit contrastChanged(contrast);
    }
}

int QGstreamerVideoWidgetControl::hue() const
{
    int hue = 0;

    if (m_videoSink && g_object_class_find_property(G_OBJECT_GET_CLASS(m_videoSink), "hue"))
        g_object_get(G_OBJECT(m_videoSink), "hue", &hue, NULL);

    return hue / 10;
}

void QGstreamerVideoWidgetControl::setHue(int hue)
{
    if (m_videoSink && g_object_class_find_property(G_OBJECT_GET_CLASS(m_videoSink), "hue")) {
        g_object_set(G_OBJECT(m_videoSink), "hue", hue * 10, NULL);

        emit hueChanged(hue);
    }
}

int QGstreamerVideoWidgetControl::saturation() const
{
    int saturation = 0;

    if (m_videoSink && g_object_class_find_property(G_OBJECT_GET_CLASS(m_videoSink), "saturation"))
        g_object_get(G_OBJECT(m_videoSink), "saturation", &saturation, NULL);

    return saturation / 10;
}

void QGstreamerVideoWidgetControl::setSaturation(int saturation)
{
    if (m_videoSink && g_object_class_find_property(G_OBJECT_GET_CLASS(m_videoSink), "saturation")) {
        g_object_set(G_OBJECT(m_videoSink), "saturation", saturation * 10, NULL);

        emit saturationChanged(saturation);
    }
}

QT_END_NAMESPACE

