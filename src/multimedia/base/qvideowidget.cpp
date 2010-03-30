/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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

#include "qvideowidget_p.h"

#include <QtMultimedia/qmediaobject.h>
#include <QtMultimedia/qmediaservice.h>
#include <QtMultimedia/qvideooutputcontrol.h>
#include <QtMultimedia/qvideowindowcontrol.h>
#include <QtMultimedia/qvideowidgetcontrol.h>

#include "qpaintervideosurface_p.h"
#include <QtMultimedia/qvideorenderercontrol.h>
#include <QtMultimedia/qvideosurfaceformat.h>
#include <qpainter.h>

#include <qapplication.h>
#include <qevent.h>
#include <qdialog.h>
#include <qboxlayout.h>
#include <qnamespace.h>


QT_BEGIN_NAMESPACE

QVideoWidgetControlBackend::QVideoWidgetControlBackend(
        QVideoWidgetControl *control, QWidget *widget)
    : m_widgetControl(control)
{
    connect(control, SIGNAL(brightnessChanged(int)), widget, SLOT(_q_brightnessChanged(int)));
    connect(control, SIGNAL(contrastChanged(int)), widget, SLOT(_q_contrastChanged(int)));
    connect(control, SIGNAL(hueChanged(int)), widget, SLOT(_q_hueChanged(int)));
    connect(control, SIGNAL(saturationChanged(int)), widget, SLOT(_q_saturationChanged(int)));
    connect(control, SIGNAL(fullScreenChanged(bool)), widget, SLOT(_q_fullScreenChanged(bool)));

    QBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(control->videoWidget());

    widget->setLayout(layout);
}

void QVideoWidgetControlBackend::setBrightness(int brightness)
{
    m_widgetControl->setBrightness(brightness);
}

void QVideoWidgetControlBackend::setContrast(int contrast)
{
    m_widgetControl->setContrast(contrast);
}

void QVideoWidgetControlBackend::setHue(int hue)
{
    m_widgetControl->setHue(hue);
}

void QVideoWidgetControlBackend::setSaturation(int saturation)
{
    m_widgetControl->setSaturation(saturation);
}

void QVideoWidgetControlBackend::setFullScreen(bool fullScreen)
{
    m_widgetControl->setFullScreen(fullScreen);
}


Qt::AspectRatioMode QVideoWidgetControlBackend::aspectRatioMode() const
{
    return m_widgetControl->aspectRatioMode();
}

void QVideoWidgetControlBackend::setAspectRatioMode(Qt::AspectRatioMode mode)
{
    m_widgetControl->setAspectRatioMode(mode);
}

QRendererVideoWidgetBackend::QRendererVideoWidgetBackend(
        QVideoRendererControl *control, QWidget *widget)
    : m_rendererControl(control)
    , m_widget(widget)
    , m_surface(new QPainterVideoSurface)
    , m_aspectRatioMode(Qt::KeepAspectRatio)
    , m_updatePaintDevice(true)
{
    connect(this, SIGNAL(brightnessChanged(int)), m_widget, SLOT(_q_brightnessChanged(int)));
    connect(this, SIGNAL(contrastChanged(int)), m_widget, SLOT(_q_contrastChanged(int)));
    connect(this, SIGNAL(hueChanged(int)), m_widget, SLOT(_q_hueChanged(int)));
    connect(this, SIGNAL(saturationChanged(int)), m_widget, SLOT(_q_saturationChanged(int)));
    connect(m_surface, SIGNAL(frameChanged()), this, SLOT(frameChanged()));
    connect(m_surface, SIGNAL(surfaceFormatChanged(QVideoSurfaceFormat)),
            this, SLOT(formatChanged(QVideoSurfaceFormat)));

    m_rendererControl->setSurface(m_surface);
}

QRendererVideoWidgetBackend::~QRendererVideoWidgetBackend()
{
    delete m_surface;
}

void QRendererVideoWidgetBackend::clearSurface()
{
    m_rendererControl->setSurface(0);
}

void QRendererVideoWidgetBackend::setBrightness(int brightness)
{
    m_surface->setBrightness(brightness);

    emit brightnessChanged(brightness);
}

void QRendererVideoWidgetBackend::setContrast(int contrast)
{
    m_surface->setContrast(contrast);

    emit contrastChanged(contrast);
}

void QRendererVideoWidgetBackend::setHue(int hue)
{
    m_surface->setHue(hue);

    emit hueChanged(hue);
}

void QRendererVideoWidgetBackend::setSaturation(int saturation)
{
    m_surface->setSaturation(saturation);

    emit saturationChanged(saturation);
}

Qt::AspectRatioMode QRendererVideoWidgetBackend::aspectRatioMode() const
{
    return m_aspectRatioMode;
}

void QRendererVideoWidgetBackend::setAspectRatioMode(Qt::AspectRatioMode mode)
{
    m_aspectRatioMode = mode;

    m_widget->updateGeometry();
}

void QRendererVideoWidgetBackend::setFullScreen(bool)
{
}

QSize QRendererVideoWidgetBackend::sizeHint() const
{
    return m_surface->surfaceFormat().sizeHint();
}

void QRendererVideoWidgetBackend::showEvent()
{
}

void QRendererVideoWidgetBackend::hideEvent(QHideEvent *)
{
#if !defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_1_CL) && !defined(QT_OPENGL_ES_1)
    m_updatePaintDevice = true;
    m_surface->setGLContext(0);
#endif
}

void QRendererVideoWidgetBackend::resizeEvent(QResizeEvent *)
{
    updateRects();
}

void QRendererVideoWidgetBackend::moveEvent(QMoveEvent *)
{
}

void QRendererVideoWidgetBackend::paintEvent(QPaintEvent *event)
{
    QPainter painter(m_widget);

    if (m_widget->testAttribute(Qt::WA_OpaquePaintEvent)) {
        QRegion borderRegion = event->region();
        borderRegion = borderRegion.subtracted(m_boundingRect);

        QBrush brush = m_widget->palette().window();

        QVector<QRect> rects = borderRegion.rects();
        for (QVector<QRect>::iterator it = rects.begin(), end = rects.end(); it != end; ++it) {
            painter.fillRect(*it, brush);
        }
    }

    if (m_surface->isActive() && m_boundingRect.intersects(event->rect())) {
        m_surface->paint(&painter, m_boundingRect, m_sourceRect);

        m_surface->setReady(true);
    } else {
 #if !defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_1_CL) && !defined(QT_OPENGL_ES_1)
        if (m_updatePaintDevice && (painter.paintEngine()->type() == QPaintEngine::OpenGL
                || painter.paintEngine()->type() == QPaintEngine::OpenGL2)) {
            m_updatePaintDevice = false;

            m_surface->setGLContext(const_cast<QGLContext *>(QGLContext::currentContext()));
            if (m_surface->supportedShaderTypes() & QPainterVideoSurface::GlslShader) {
                m_surface->setShaderType(QPainterVideoSurface::GlslShader);
            } else {
                m_surface->setShaderType(QPainterVideoSurface::FragmentProgramShader);
            }
        }
#endif
    }

}

void QRendererVideoWidgetBackend::formatChanged(const QVideoSurfaceFormat &format)
{
    m_nativeSize = format.sizeHint();

    updateRects();

    m_widget->updateGeometry();
    m_widget->update();
}

void QRendererVideoWidgetBackend::frameChanged()
{
    m_widget->update(m_boundingRect);
}

void QRendererVideoWidgetBackend::updateRects()
{
    QRect rect = m_widget->rect();

    if (m_nativeSize.isEmpty()) {
        m_boundingRect = QRect();
    } else if (m_aspectRatioMode == Qt::IgnoreAspectRatio) {
        m_boundingRect = rect;
        m_sourceRect = QRectF(0, 0, 1, 1);
    } else if (m_aspectRatioMode == Qt::KeepAspectRatio) {
        QSize size = m_nativeSize;
        size.scale(rect.size(), Qt::KeepAspectRatio);

        m_boundingRect = QRect(0, 0, size.width(), size.height());
        m_boundingRect.moveCenter(rect.center());

        m_sourceRect = QRectF(0, 0, 1, 1);
    } else if (m_aspectRatioMode == Qt::KeepAspectRatioByExpanding) {
        m_boundingRect = rect;

        QSizeF size = rect.size();
        size.scale(m_nativeSize, Qt::KeepAspectRatio);

        m_sourceRect = QRectF(
                0, 0, size.width() / m_nativeSize.width(), size.height() / m_nativeSize.height());
        m_sourceRect.moveCenter(QPointF(0.5, 0.5));
    }
}

QWindowVideoWidgetBackend::QWindowVideoWidgetBackend(QVideoWindowControl *control, QWidget *widget)
    : m_windowControl(control)
    , m_widget(widget)
    , m_aspectRatioMode(Qt::KeepAspectRatio)
{
    connect(control, SIGNAL(brightnessChanged(int)), m_widget, SLOT(_q_brightnessChanged(int)));
    connect(control, SIGNAL(contrastChanged(int)), m_widget, SLOT(_q_contrastChanged(int)));
    connect(control, SIGNAL(hueChanged(int)), m_widget, SLOT(_q_hueChanged(int)));
    connect(control, SIGNAL(saturationChanged(int)), m_widget, SLOT(_q_saturationChanged(int)));
    connect(control, SIGNAL(fullScreenChanged(bool)), m_widget, SLOT(_q_fullScreenChanged(bool)));
    connect(control, SIGNAL(nativeSizeChanged()), m_widget, SLOT(_q_dimensionsChanged()));
}

QWindowVideoWidgetBackend::~QWindowVideoWidgetBackend()
{
}

void QWindowVideoWidgetBackend::setBrightness(int brightness)
{
    m_windowControl->setBrightness(brightness);
}

void QWindowVideoWidgetBackend::setContrast(int contrast)
{
    m_windowControl->setContrast(contrast);
}

void QWindowVideoWidgetBackend::setHue(int hue)
{
    m_windowControl->setHue(hue);
}

void QWindowVideoWidgetBackend::setSaturation(int saturation)
{
    m_windowControl->setSaturation(saturation);
}

void QWindowVideoWidgetBackend::setFullScreen(bool fullScreen)
{
    m_windowControl->setFullScreen(fullScreen);
}

Qt::AspectRatioMode QWindowVideoWidgetBackend::aspectRatioMode() const
{
    return m_windowControl->aspectRatioMode();
}

void QWindowVideoWidgetBackend::setAspectRatioMode(Qt::AspectRatioMode mode)
{
    m_windowControl->setAspectRatioMode(mode);
}

QSize QWindowVideoWidgetBackend::sizeHint() const
{
    return m_windowControl->nativeSize();
}

void QWindowVideoWidgetBackend::showEvent()
{
    m_windowControl->setWinId(m_widget->winId());

    m_windowControl->setDisplayRect(m_widget->rect());
}

void QWindowVideoWidgetBackend::hideEvent(QHideEvent *)
{
}

void QWindowVideoWidgetBackend::moveEvent(QMoveEvent *)
{
    m_windowControl->setDisplayRect(m_widget->rect());
}

void QWindowVideoWidgetBackend::resizeEvent(QResizeEvent *)
{
    m_windowControl->setDisplayRect(m_widget->rect());
}

void QWindowVideoWidgetBackend::paintEvent(QPaintEvent *event)
{
    if (m_widget->testAttribute(Qt::WA_OpaquePaintEvent)) {
        QPainter painter(m_widget);

        painter.fillRect(event->rect(), m_widget->palette().window());
    }

    m_windowControl->repaint();

    event->accept();
}

void QVideoWidgetPrivate::setCurrentControl(QVideoWidgetControlInterface *control)
{
    if (currentControl != control) {
        currentControl = control;

        currentControl->setBrightness(brightness);
        currentControl->setContrast(contrast);
        currentControl->setHue(hue);
        currentControl->setSaturation(saturation);
        currentControl->setAspectRatioMode(aspectRatioMode);
    }
}

void QVideoWidgetPrivate::show()
{
    if (outputControl) {
        if (widgetBackend != 0) {
            setCurrentControl(widgetBackend);
            outputControl->setOutput(QVideoOutputControl::WidgetOutput);
        } else if (windowBackend != 0 && (q_func()->window() == 0
                || !q_func()->window()->testAttribute(Qt::WA_DontShowOnScreen))) {
            windowBackend->showEvent();
            currentBackend = windowBackend;
            setCurrentControl(windowBackend);
            outputControl->setOutput(QVideoOutputControl::WindowOutput);
        } else if (rendererBackend != 0) {
            rendererBackend->showEvent();
            currentBackend = rendererBackend;
            setCurrentControl(rendererBackend);
            outputControl->setOutput(QVideoOutputControl::RendererOutput);
        } else {
            outputControl->setOutput(QVideoOutputControl::NoOutput);
        }
    }
}

void QVideoWidgetPrivate::clearService()
{
    if (service) {
        QObject::disconnect(service, SIGNAL(destroyed()), q_func(), SLOT(_q_serviceDestroyed()));

        if (outputControl)
            outputControl->setOutput(QVideoOutputControl::NoOutput);

        if (widgetBackend) {
            QLayout *layout = q_func()->layout();

            for (QLayoutItem *item = layout->takeAt(0); item; item = layout->takeAt(0)) {
                item->widget()->setParent(0);
                delete item;
            }
            delete layout;

            delete widgetBackend;
            widgetBackend = 0;
        }

        delete windowBackend;
        windowBackend = 0;

        if (rendererBackend) {
            rendererBackend->clearSurface();

            delete rendererBackend;
            rendererBackend = 0;
        }

        currentBackend = 0;
        currentControl = 0;
        outputControl = 0;
        service = 0;
    }
}

void QVideoWidgetPrivate::_q_serviceDestroyed()
{
    if (widgetBackend) {
        delete q_func()->layout();

        delete widgetBackend;
        widgetBackend = 0;
    }

    delete windowBackend;
    windowBackend = 0;

    delete rendererBackend;
    rendererBackend = 0;

    currentControl = 0;
    currentBackend = 0;
    outputControl = 0;
    service = 0;
}

void QVideoWidgetPrivate::_q_mediaObjectDestroyed()
{
    mediaObject = 0;
    clearService();
}

void QVideoWidgetPrivate::_q_brightnessChanged(int b)
{
    if (b != brightness)
        emit q_func()->brightnessChanged(brightness = b);
}

void QVideoWidgetPrivate::_q_contrastChanged(int c)
{
    if (c != contrast)
        emit q_func()->contrastChanged(contrast = c);
}

void QVideoWidgetPrivate::_q_hueChanged(int h)
{
    if (h != hue)
        emit q_func()->hueChanged(hue = h);
}

void QVideoWidgetPrivate::_q_saturationChanged(int s)
{
    if (s != saturation)
        emit q_func()->saturationChanged(saturation = s);
}


void QVideoWidgetPrivate::_q_fullScreenChanged(bool fullScreen)
{
    if (!fullScreen && q_func()->isFullScreen())
        q_func()->showNormal();
}

void QVideoWidgetPrivate::_q_dimensionsChanged()
{
    q_func()->updateGeometry();
    q_func()->update();
}

/*!
    \class QVideoWidget
    \preliminary
    \since 4.7
    \brief The QVideoWidget class provides a widget which presents video
    produced by a media object.
    \ingroup multimedia

    Attaching a QVideoWidget to a QMediaObject allows it to display the
    video or image output of that media object.  A QVideoWidget is attached
    to media object by passing a pointer to the QMediaObject in its
    constructor, and detached by destroying the QVideoWidget.

    \code
        player = new QMediaPlayer;

        widget = new QVideoWidget(player);
        widget->show();

        player->setMedia(QUrl("http://example.com/movie.mp4"));
        player->play();
    \endcode

    \bold {Note}: Only a single display output can be attached to a media
    object at one time.

    \sa QMediaObject, QMediaPlayer, QGraphicsVideoItem
*/

/*!
    Constructs a new video widget.

    The \a parent is passed to QWidget.
*/
QVideoWidget::QVideoWidget(QWidget *parent)
    : QWidget(parent, 0)
    , d_ptr(new QVideoWidgetPrivate)
{
    d_ptr->q_ptr = this;
}

/*!
    Destroys a video widget.
*/
QVideoWidget::~QVideoWidget()
{
    setMediaObject(0);
    delete d_ptr;
}

/*!
    \property QVideoWidget::mediaObject
    \brief the media object which provides the video displayed by a widget.
*/

QMediaObject *QVideoWidget::mediaObject() const
{
    return d_func()->mediaObject;
}

void QVideoWidget::setMediaObject(QMediaObject *object)
{
    Q_D(QVideoWidget);

    if (object == d->mediaObject)
        return;

    if (d->mediaObject) {
        disconnect(d->mediaObject, SIGNAL(destroyed()), this, SLOT(_q_mediaObjectDestroyed()));
        d->mediaObject->unbind(this);
    }

    d->clearService();

    d->mediaObject = object;

    if (d->mediaObject) {
        d->service = d->mediaObject->service();

        connect(d->mediaObject, SIGNAL(destroyed()), this, SLOT(_q_mediaObjectDestroyed()));
        d->mediaObject->bind(this);
    }

    if (d->service) {
        connect(d->service, SIGNAL(destroyed()), SLOT(_q_serviceDestroyed()));

        d->outputControl = qobject_cast<QVideoOutputControl *>(
                d->service->control(QVideoOutputControl_iid));

        QVideoWidgetControl *widgetControl = qobject_cast<QVideoWidgetControl *>(
                d->service->control(QVideoWidgetControl_iid));

        if (widgetControl != 0) {
            d->widgetBackend = new QVideoWidgetControlBackend(widgetControl, this);
        } else {
            QVideoWindowControl *windowControl = qobject_cast<QVideoWindowControl *>(
                    d->service->control(QVideoWindowControl_iid));

            if (windowControl != 0)
                d->windowBackend = new QWindowVideoWidgetBackend(windowControl, this);

            QVideoRendererControl *rendererControl = qobject_cast<QVideoRendererControl *>(
                    d->service->control(QVideoRendererControl_iid));

            if (rendererControl != 0)
                d->rendererBackend = new QRendererVideoWidgetBackend(rendererControl, this);
        }

        if (isVisible())
            d->show();
    }
}

/*!
    \property QVideoWidget::aspectRatioMode
    \brief how video is scaled with respect to its aspect ratio.
*/

Qt::AspectRatioMode QVideoWidget::aspectRatioMode() const
{
    return d_func()->aspectRatioMode;
}

void QVideoWidget::setAspectRatioMode(Qt::AspectRatioMode mode)
{
    Q_D(QVideoWidget);

    if (d->currentControl) {
        d->currentControl->setAspectRatioMode(mode);
        d->aspectRatioMode = d->currentControl->aspectRatioMode();
    } else {
        d->aspectRatioMode = mode;
    }
}

/*!
    \property QVideoWidget::fullScreen
    \brief whether video display is confined to a window or is fullScreen.
*/

void QVideoWidget::setFullScreen(bool fullScreen)
{
    Q_D(QVideoWidget);

    if (fullScreen) {
        Qt::WindowFlags flags = windowFlags();

        d->nonFullScreenFlags = flags & (Qt::Window | Qt::SubWindow);
        flags |= Qt::Window;
        flags &= ~Qt::SubWindow;
        setWindowFlags(flags);
        
        showFullScreen();
    } else {
        showNormal();
    }
}

/*!
    \fn QVideoWidget::fullScreenChanged(bool fullScreen)

    Signals that the \a fullScreen mode of a video widget has changed.

    \sa fullScreen
*/

/*!
    \property QVideoWidget::brightness
    \brief an adjustment to the brightness of displayed video.

    Valid brightness values range between -100 and 100, the default is 0.
*/

int QVideoWidget::brightness() const
{
    return d_func()->brightness;
}

void QVideoWidget::setBrightness(int brightness)
{
    Q_D(QVideoWidget);

    int boundedBrightness = qBound(-100, brightness, 100);

    if (d->currentControl)
        d->currentControl->setBrightness(boundedBrightness);
    else if (d->brightness != boundedBrightness)
        emit brightnessChanged(d->brightness = boundedBrightness);
}

/*!
    \fn QVideoWidget::brightnessChanged(int brightness)

    Signals that a video widgets's \a brightness adjustment has changed.

    \sa brightness
*/

/*!
    \property QVideoWidget::contrast
    \brief an adjustment to the contrast of displayed video.

    Valid contrast values range between -100 and 100, the default is 0.

*/

int QVideoWidget::contrast() const
{
    return d_func()->contrast;
}

void QVideoWidget::setContrast(int contrast)
{
    Q_D(QVideoWidget);

    int boundedContrast = qBound(-100, contrast, 100);

    if (d->currentControl)
        d->currentControl->setContrast(boundedContrast);
    else if (d->contrast != boundedContrast)
        emit contrastChanged(d->contrast = boundedContrast);
}

/*!
    \fn QVideoWidget::contrastChanged(int contrast)

    Signals that a video widgets's \a contrast adjustment has changed.

    \sa contrast
*/

/*!
    \property QVideoWidget::hue
    \brief an adjustment to the hue of displayed video.

    Valid hue values range between -100 and 100, the default is 0.
*/

int QVideoWidget::hue() const
{
    return d_func()->hue;
}

void QVideoWidget::setHue(int hue)
{
    Q_D(QVideoWidget);

    int boundedHue = qBound(-100, hue, 100);

    if (d->currentControl)
        d->currentControl->setHue(boundedHue);
    else if (d->hue != boundedHue)
        emit hueChanged(d->hue = boundedHue);
}

/*!
    \fn QVideoWidget::hueChanged(int hue)

    Signals that a video widgets's \a hue has changed.

    \sa hue
*/

/*!
    \property QVideoWidget::saturation
    \brief an adjustment to the saturation of displayed video.

    Valid saturation values range between -100 and 100, the default is 0.
*/

int QVideoWidget::saturation() const
{
    return d_func()->saturation;
}

void QVideoWidget::setSaturation(int saturation)
{
    Q_D(QVideoWidget);

    int boundedSaturation = qBound(-100, saturation, 100);

    if (d->currentControl)
        d->currentControl->setSaturation(boundedSaturation);
    else if (d->saturation != boundedSaturation)
        emit saturationChanged(d->saturation = boundedSaturation);

}

/*!
    \fn QVideoWidget::saturationChanged(int saturation)

    Signals that a video widgets's \a saturation has changed.

    \sa saturation
*/

/*!
  Returns the size hint for the current back end,
  if there is one, or else the size hint from QWidget.
 */
QSize QVideoWidget::sizeHint() const
{
    Q_D(const QVideoWidget);

    if (d->currentBackend)
        return d->currentBackend->sizeHint();
    else
        return QWidget::sizeHint();


}

/*!
    \reimp
    \internal
 */
bool QVideoWidget::event(QEvent *event)
{
    Q_D(QVideoWidget);

    if (event->type() == QEvent::WindowStateChange) {
        Qt::WindowFlags flags = windowFlags();

        if (windowState() & Qt::WindowFullScreen) {
            if (d->currentControl)
                d->currentControl->setFullScreen(true);

            if (!d->wasFullScreen)
                emit fullScreenChanged(d->wasFullScreen = true);
        } else {
            if (d->currentControl)
                d->currentControl->setFullScreen(false);

            if (d->wasFullScreen) {
                flags &= ~(Qt::Window | Qt::SubWindow); //clear the flags...
                flags |= d->nonFullScreenFlags; //then we reset the flags (window and subwindow)
                setWindowFlags(flags);

                emit fullScreenChanged(d->wasFullScreen = false);
            }
        }
    }
    return QWidget::event(event);
}

/*!
  Handles the show \a event.
 */
void QVideoWidget::showEvent(QShowEvent *event)
{
    Q_D(QVideoWidget);

    QWidget::showEvent(event);

    d->show();

}

/*!

  Handles the hide \a event.
*/
void QVideoWidget::hideEvent(QHideEvent *event)
{
    Q_D(QVideoWidget);

    if (d->currentBackend)
        d->currentBackend->hideEvent(event);

    QWidget::hideEvent(event);
}

/*!
  Handles the resize \a event.
 */
void QVideoWidget::resizeEvent(QResizeEvent *event)
{
    Q_D(QVideoWidget);

    QWidget::resizeEvent(event);

    if (d->currentBackend)
        d->currentBackend->resizeEvent(event);
}

/*!
  Handles the move \a event.
 */
void QVideoWidget::moveEvent(QMoveEvent *event)
{
    Q_D(QVideoWidget);

    if (d->currentBackend)
        d->currentBackend->moveEvent(event);
}

/*!
  Handles the paint \a event.
 */
void QVideoWidget::paintEvent(QPaintEvent *event)
{
    Q_D(QVideoWidget);

    if (d->currentBackend) {
        d->currentBackend->paintEvent(event);
    } else if (testAttribute(Qt::WA_OpaquePaintEvent)) {
        QPainter painter(this);

        painter.fillRect(event->rect(), palette().window());
    }
}

#include "moc_qvideowidget.cpp"
#include "moc_qvideowidget_p.cpp"

QT_END_NAMESPACE

