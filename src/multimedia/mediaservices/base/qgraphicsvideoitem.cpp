/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMediaservies module of the Qt Toolkit.
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

#include <QtMediaServices/qgraphicsvideoitem.h>

#include <QtMediaServices/qmediaobject.h>
#include <QtMediaServices/qmediaservice.h>
#include <QtMediaServices/private/qpaintervideosurface_p.h>
#include <QtMediaServices/qvideooutputcontrol.h>
#include <QtMediaServices/qvideorenderercontrol.h>
#include <QtMultimedia/qvideosurfaceformat.h>

#if !defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_1_CL) && !defined(QT_OPENGL_ES_1)
#include <QtOpenGL/qgl.h>
#endif

#ifndef QT_NO_GRAPHICSVIEW

QT_BEGIN_NAMESPACE


class QGraphicsVideoItemPrivate
{
public:
    QGraphicsVideoItemPrivate()
        : q_ptr(0)
        , surface(0)
        , mediaObject(0)
        , service(0)
        , outputControl(0)
        , rendererControl(0)
        , aspectRatioMode(Qt::KeepAspectRatio)
        , updatePaintDevice(true)
        , rect(0.0, 0.0, 320, 240)
    {
    }

    QGraphicsVideoItem *q_ptr;

    QPainterVideoSurface *surface;
    QMediaObject *mediaObject;
    QMediaService *service;
    QVideoOutputControl *outputControl;
    QVideoRendererControl *rendererControl;
    Qt::AspectRatioMode aspectRatioMode;
    bool updatePaintDevice;
    QRectF rect;
    QRectF boundingRect;
    QRectF sourceRect;
    QSizeF nativeSize;

    void clearService();
    void updateRects();

    void _q_present();
    void _q_formatChanged(const QVideoSurfaceFormat &format);
    void _q_serviceDestroyed();
    void _q_mediaObjectDestroyed();
};

void QGraphicsVideoItemPrivate::clearService()
{
    if (outputControl) {
        outputControl->setOutput(QVideoOutputControl::NoOutput);
        outputControl = 0;
    }
    if (rendererControl) {
        surface->stop();
        rendererControl->setSurface(0);
        rendererControl = 0;
    }
    if (service) {
        QObject::disconnect(service, SIGNAL(destroyed()), q_ptr, SLOT(_q_serviceDestroyed()));
        service = 0;
    }
}

void QGraphicsVideoItemPrivate::updateRects()
{
    q_ptr->prepareGeometryChange();

    if (nativeSize.isEmpty()) {
        boundingRect = QRectF();
    } else if (aspectRatioMode == Qt::IgnoreAspectRatio) {
        boundingRect = rect;
        sourceRect = QRectF(0, 0, 1, 1);
    } else if (aspectRatioMode == Qt::KeepAspectRatio) {
        QSizeF size = nativeSize;
        size.scale(rect.size(), Qt::KeepAspectRatio);

        boundingRect = QRectF(0, 0, size.width(), size.height());
        boundingRect.moveCenter(rect.center());

        sourceRect = QRectF(0, 0, 1, 1);
    } else if (aspectRatioMode == Qt::KeepAspectRatioByExpanding) {
        boundingRect = rect;

        QSizeF size = rect.size();
        size.scale(nativeSize, Qt::KeepAspectRatio);

        sourceRect = QRectF(
                0, 0, size.width() / nativeSize.width(), size.height() / nativeSize.height());
        sourceRect.moveCenter(QPointF(0.5, 0.5));
    }
}

void QGraphicsVideoItemPrivate::_q_present()
{
    if (q_ptr->isObscured()) {
        q_ptr->update(boundingRect);
        surface->setReady(true);
    } else {
        q_ptr->update(boundingRect);
    }
}

void QGraphicsVideoItemPrivate::_q_formatChanged(const QVideoSurfaceFormat &format)
{
    nativeSize = format.sizeHint();

    updateRects();

    emit q_ptr->nativeSizeChanged(nativeSize);
}

void QGraphicsVideoItemPrivate::_q_serviceDestroyed()
{
    rendererControl = 0;
    outputControl = 0;
    service = 0;

    surface->stop();
}

void QGraphicsVideoItemPrivate::_q_mediaObjectDestroyed()
{
    mediaObject = 0;

    clearService();
}

/*!
    \class QGraphicsVideoItem
    \brief The QGraphicsVideoItem class provides a graphics item which display video produced by a QMediaObject.

    \since 4.7

    \ingroup multimedia

    Attaching a QGraphicsVideoItem to a QMediaObject allows it to display
    the video or image output of that media object.  A QGraphicsVideoItem
    is attached to a media object by passing a pointer to the QMediaObject
    to the setMediaObject() function.

    \code
    player = new QMediaPlayer(this);

    QGraphicsVideoItem *item = new QGraphicsVideoItem;
    item->setMediaObject(player);
    graphicsView->scence()->addItem(item);
    graphicsView->show();

    player->setMedia(video);
    player->play();
    \endcode

    \bold {Note}: Only a single display output can be attached to a media
    object at one time.

    \sa QMediaObject, QMediaPlayer, QVideoWidget
*/

/*!
    Constructs a graphics item that displays video.

    The \a parent is passed to QGraphicsItem.
*/
QGraphicsVideoItem::QGraphicsVideoItem(QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , d_ptr(new QGraphicsVideoItemPrivate)
{
    d_ptr->q_ptr = this;
    d_ptr->surface = new QPainterVideoSurface;

    connect(d_ptr->surface, SIGNAL(frameChanged()), this, SLOT(_q_present()));
    connect(d_ptr->surface, SIGNAL(surfaceFormatChanged(QVideoSurfaceFormat)),
            this, SLOT(_q_formatChanged(QVideoSurfaceFormat)));
}

/*!
    Destroys a video graphics item.
*/
QGraphicsVideoItem::~QGraphicsVideoItem()
{
    if (d_ptr->outputControl)
        d_ptr->outputControl->setOutput(QVideoOutputControl::NoOutput);

    if (d_ptr->rendererControl)
        d_ptr->rendererControl->setSurface(0);

    delete d_ptr->surface;
    delete d_ptr;
}

/*!
    \property QGraphicsVideoItem::mediaObject
    \brief the media object which provides the video displayed by a graphics
    item.
*/

QMediaObject *QGraphicsVideoItem::mediaObject() const
{
    return d_func()->mediaObject;
}

void QGraphicsVideoItem::setMediaObject(QMediaObject *object)
{
    Q_D(QGraphicsVideoItem);

    if (object == d->mediaObject)
        return;

    d->clearService();

    if (d->mediaObject) {
        disconnect(d->mediaObject, SIGNAL(destroyed()), this, SLOT(_q_mediaObjectDestroyed()));
        d->mediaObject->unbind(this);
    }

    d->mediaObject = object;

    if (d->mediaObject) {
        d->mediaObject->bind(this);

        connect(d->mediaObject, SIGNAL(destroyed()), this, SLOT(_q_mediaObjectDestroyed()));

        d->service = d->mediaObject->service();

        if (d->service) {
            connect(d->service, SIGNAL(destroyed()), this, SLOT(_q_serviceDestroyed()));

            d->outputControl = qobject_cast<QVideoOutputControl *>(
                    d->service->control(QVideoOutputControl_iid));
            d->rendererControl = qobject_cast<QVideoRendererControl *>(
                    d->service->control(QVideoRendererControl_iid));

            if (d->outputControl != 0 && d->rendererControl != 0) {
                d->rendererControl->setSurface(d->surface);

                if (isVisible())
                    d->outputControl->setOutput(QVideoOutputControl::RendererOutput);
            }
        }
    }
}

/*!
    \property QGraphicsVideoItem::aspectRatioMode
    \brief how a video is scaled to fit the graphics item's size.
*/

Qt::AspectRatioMode QGraphicsVideoItem::aspectRatioMode() const
{
    return d_func()->aspectRatioMode;
}

void QGraphicsVideoItem::setAspectRatioMode(Qt::AspectRatioMode mode)
{
    Q_D(QGraphicsVideoItem);

    d->aspectRatioMode = mode;
    d->updateRects();
}

/*!
    \property QGraphicsVideoItem::offset
    \brief the video item's offset.

    QGraphicsVideoItem will draw video using the offset for its top left
    corner.
*/

QPointF QGraphicsVideoItem::offset() const
{
    return d_func()->rect.topLeft();
}

void QGraphicsVideoItem::setOffset(const QPointF &offset)
{
    Q_D(QGraphicsVideoItem);

    d->rect.moveTo(offset);
    d->updateRects();
}

/*!
    \property QGraphicsVideoItem::size
    \brief the video item's size.

    QGraphicsVideoItem will draw video scaled to fit size according to its
    fillMode.
*/

QSizeF QGraphicsVideoItem::size() const
{
    return d_func()->rect.size();
}

void QGraphicsVideoItem::setSize(const QSizeF &size)
{
    Q_D(QGraphicsVideoItem);

    d->rect.setSize(size.isValid() ? size : QSizeF(0, 0));
    d->updateRects();
}

/*!
    \property QGraphicsVideoItem::nativeSize
    \brief the native size of the video.
*/

QSizeF QGraphicsVideoItem::nativeSize() const
{
    return d_func()->nativeSize;
}

/*!
    \fn QGraphicsVideoItem::nativeSizeChanged(const QSizeF &size)

    Signals that the native \a size of the video has changed.
*/

/*!
    \reimp
*/
QRectF QGraphicsVideoItem::boundingRect() const
{
    return d_func()->boundingRect;
}

/*!
    \reimp
*/
void QGraphicsVideoItem::paint(
        QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_D(QGraphicsVideoItem);

    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (d->surface && d->surface->isActive()) {
        d->surface->paint(painter, d->boundingRect, d->sourceRect);
        d->surface->setReady(true);
#if !defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_1_CL) && !defined(QT_OPENGL_ES_1)
    } else if (d->updatePaintDevice && (painter->paintEngine()->type() == QPaintEngine::OpenGL
            || painter->paintEngine()->type() == QPaintEngine::OpenGL2)) {
        d->updatePaintDevice = false;

        d->surface->setGLContext(const_cast<QGLContext *>(QGLContext::currentContext()));
        if (d->surface->supportedShaderTypes() & QPainterVideoSurface::GlslShader) {
            d->surface->setShaderType(QPainterVideoSurface::GlslShader);
        } else {
            d->surface->setShaderType(QPainterVideoSurface::FragmentProgramShader);
        }
#endif
    }
}

/*!
    \reimp

    \internal
*/
QVariant QGraphicsVideoItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    return QGraphicsObject::itemChange(change, value);
}

/*!
    \reimp

    \internal
*/
bool QGraphicsVideoItem::event(QEvent *event)
{
    return QGraphicsObject::event(event);
}

/*!
    \reimp

    \internal
*/
bool QGraphicsVideoItem::sceneEvent(QEvent *event)
{
    return QGraphicsObject::sceneEvent(event);
}

QT_END_NAMESPACE

#endif // QT_NO_GRAPHICSVIEW

#include "moc_qgraphicsvideoitem.cpp"
