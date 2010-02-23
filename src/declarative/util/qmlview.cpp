/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qmlview.h"

#include "qperformancelog_p_p.h"
#include "qfxperf_p_p.h"

#include <qml.h>
#include <qmlgraphicsitem.h>
#include <qmlengine.h>
#include <qmlcontext.h>
#include <qmldebug_p.h>
#include <qmldebugservice_p.h>
#include <qmlglobal_p.h>

#include <qscriptvalueiterator.h>
#include <qdebug.h>
#include <qtimer.h>
#include <qevent.h>
#include <qdir.h>
#include <qcoreapplication.h>
#include <qfontdatabase.h>
#include <qicon.h>
#include <qurl.h>
#include <qboxlayout.h>
#include <qbasictimer.h>
#include <QtCore/qabstractanimation.h>
#include <private/qgraphicsview_p.h>

QT_BEGIN_NAMESPACE

DEFINE_BOOL_CONFIG_OPTION(frameRateDebug, QML_SHOW_FRAMERATE)

class QmlViewDebugServer;
class FrameBreakAnimation : public QAbstractAnimation
{
public:
    FrameBreakAnimation(QmlViewDebugServer *s)
    : QAbstractAnimation((QObject*)s), server(s)
    {
        start();
    }

    virtual int duration() const { return -1; }
    virtual void updateCurrentTime(int msecs);

private:
    QmlViewDebugServer *server;
};

class QmlViewDebugServer : public QmlDebugService
{
public:
    QmlViewDebugServer(QObject *parent = 0) : QmlDebugService(QLatin1String("CanvasFrameRate"), parent), breaks(0)
    {
        timer.start();
        new FrameBreakAnimation(this);
    }

    void addTiming(int pe, int tbf)
    {
        if (!isEnabled())
            return;

        bool isFrameBreak = breaks > 1;
        breaks = 0;
        int e = timer.elapsed();
        QByteArray data;
        QDataStream ds(&data, QIODevice::WriteOnly);
        ds << (int)pe << (int)tbf << (int)e
           << (bool)isFrameBreak;
        sendMessage(data);
    }

    void frameBreak() { ++breaks; }

private:
    QTime timer;
    int breaks;
};

Q_GLOBAL_STATIC(QmlViewDebugServer, qfxViewDebugServer);

void FrameBreakAnimation::updateCurrentTime(int msecs)
{
    Q_UNUSED(msecs);
    server->frameBreak();
}

class QmlViewPrivate
{
public:
    QmlViewPrivate(QmlView *view)
        : q(view), root(0), component(0), resizeMode(QmlView::SizeViewToRootObject) {}

    QmlView *q;

    QGuard<QGraphicsObject> root;
    QGuard<QmlGraphicsItem> qmlRoot;

    QUrl source;

    QmlEngine engine;
    QmlComponent *component;
    QBasicTimer resizetimer;

    mutable QSize initialSize;
    QmlView::ResizeMode resizeMode;
    QTime frameTimer;

    void init();

    QGraphicsScene scene;
};

/*!
    \class QmlView
  \since 4.7
    \brief The QmlView class provides a widget for displaying a Qt Declarative user interface.

    Any QGraphicsObject or QmlGraphicsItem
    created via QML can be placed on a standard QGraphicsScene and viewed with a standard
    QGraphicsView.

    QmlView is a QGraphicsView subclass provided as a convenience for displaying QML
    files, and connecting between QML and C++ Qt objects.

    QmlView performs the following functions:

    \list
    \o Manages QmlComponent loading and object creation.
    \o Initializes QGraphicsView for optimal performance with QML:
        \list
        \o QGraphicsView::setOptimizationFlags(QGraphicsView::DontSavePainterState);
        \o QGraphicsView::setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
        \o QGraphicsScene::setItemIndexMethod(QGraphicsScene::NoIndex);
        \endlist
    \o Initializes QGraphicsView for QML key handling:
        \list
        \o QGraphicsView::viewport()->setFocusPolicy(Qt::NoFocus);
        \o QGraphicsScene::setStickyFocus(true);
        \endlist
    \endlist

    Typical usage:
    \code
    ...
    QmlView *view = new QmlView(this);
    vbox->addWidget(view);

    QUrl url(fileName);
    view->setSource(url);
    ...
    view->execute();
    ...
    view->show();
    \endcode

    To receive errors related to loading and executing QML with QmlView,
    you can connect to the statusChanged() signal and monitor for QmlView::Error.
    The errors are available via QmlView::errors().
*/


/*! \fn void QmlView::sceneResized(QSize size)
  This signal is emitted when the view is resized to \a size.
*/

/*! \fn void QmlView::statusChanged(QmlView::Status status)
    This signal is emitted when the component's current \l{QmlView::Status} {status} changes.
*/

/*!
  \fn QmlView::QmlView(QWidget *parent)
  
  Constructs a QmlView with the given \a parent.
*/
QmlView::QmlView(QWidget *parent)
: QGraphicsView(parent), d(new QmlViewPrivate(this))
{
    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    d->init();
}

void QmlViewPrivate::init()
{
#ifdef Q_ENABLE_PERFORMANCE_LOG
    {
        QmlPerfTimer<QmlPerf::FontDatabase> perf;
        QFontDatabase database;
    }
#endif

    q->setScene(&scene);

    q->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    q->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    q->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    q->setFrameStyle(0);

    // These seem to give the best performance
    q->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    scene.setItemIndexMethod(QGraphicsScene::NoIndex);
    q->viewport()->setFocusPolicy(Qt::NoFocus);

    scene.setStickyFocus(true);  //### needed for correct focus handling
}

/*!
  The destructor clears the view's \l {QGraphicsObject} {items} and
  deletes the internal representation.
 */
QmlView::~QmlView()
{
    delete d->root;
}

/*!
    Sets the source to the \a url.

    Call \l execute() to load the QML and instantiate the component.

    \sa execute()
 */
void QmlView::setSource(const QUrl& url)
{
    d->source = url;
}

/*!
  Returns the source URL, if set.

  \sa setSource()
 */
QUrl QmlView::source() const
{
    return d->source;
}

/*!
  Returns a pointer to the QmlEngine used for instantiating
  QML Components.
 */
QmlEngine* QmlView::engine()
{
    return &d->engine;
}

/*!
  This function returns the root of the context hierarchy.  Each QML
  component is instantiated in a QmlContext.  QmlContext's are
  essential for passing data to QML components.  In QML, contexts are
  arranged hierarchically and this hierarchy is managed by the
  QmlEngine.
 */
QmlContext* QmlView::rootContext()
{
    return d->engine.rootContext();
}

/*!
    Loads and instantiates the QML component set by the \l setSource() method.

    \sa setSource()
*/
void QmlView::execute()
{
    delete d->root;
    delete d->component;
    d->component = new QmlComponent(&d->engine, d->source, this);

    if (!d->component->isLoading()) {
        continueExecute();
    } else {
        connect(d->component, SIGNAL(statusChanged(QmlComponent::Status)), this, SLOT(continueExecute()));
    }
}

/*!
  \enum QmlView::Status

    Specifies the loading status of the QmlView.

    \value Null This QmlView has no source set.
    \value Ready This QmlView has loaded and created the QML component.
    \value Loading This QmlView is loading network data.
    \value Error An error has occured.  Calling errorDescription() to retrieve a description.
*/

/*!
    \property QmlView::status
    The component's current \l{QmlView::Status} {status}.
*/

QmlView::Status QmlView::status() const
{
    if (!d->component)
        return QmlView::Null;

    return QmlView::Status(d->component->status());
}

/*!
    Return the list of errors that occured during the last compile or create
    operation.  An empty list is returned if isError() is not set.
*/
QList<QmlError> QmlView::errors() const
{
    if (d->component)
        return d->component->errors();
    return QList<QmlError>();
}


/*!
    \property QmlView::resizeMode
    \brief whether the view should resize the canvas contents

    If this property is set to SizeViewToRootObject (the default), the view
    resizes with the root item in the QML.

    If this property is set to SizeRootObjectToView, the view will
    automatically resize the root item.

    Regardless of this property, the sizeHint of the view
    is the initial size of the root item. Note though that
    since QML may load dynamically, that size may change.

    \sa initialSize()
*/

void QmlView::setResizeMode(ResizeMode mode)
{
    if (d->resizeMode == mode)
        return;

    d->resizeMode = mode;
    if (d->qmlRoot) {
        if (d->resizeMode == SizeRootObjectToView) {
            d->qmlRoot->setWidth(width());
            d->qmlRoot->setHeight(height());
        } else {
            d->qmlRoot->setWidth(d->initialSize.width());
            d->qmlRoot->setHeight(d->initialSize.height());
        }
    }
}

QmlView::ResizeMode QmlView::resizeMode() const
{
    return d->resizeMode;
}

/*!
  \internal
 */
void QmlView::continueExecute()
{

    disconnect(d->component, SIGNAL(statusChanged(QmlComponent::Status)), this, SLOT(continueExecute()));

    if (d->component->isError()) {
        QList<QmlError> errorList = d->component->errors();
        foreach (const QmlError &error, errorList) {
            qWarning() << error;
        }
        emit statusChanged(status());
        return;
    }

    QObject *obj = d->component->create();

    if(d->component->isError()) {
        QList<QmlError> errorList = d->component->errors();
        foreach (const QmlError &error, errorList) {
            qWarning() << error;
        }
        emit statusChanged(status());
        return;
    }

    if (obj) {
        if (QmlGraphicsItem *item = qobject_cast<QmlGraphicsItem *>(obj)) {

            d->scene.addItem(item);

            QPerformanceLog::displayData();
            QPerformanceLog::clear();
            d->root = item;
            d->qmlRoot = item;
            connect(item, SIGNAL(widthChanged()), this, SLOT(sizeChanged()));
            connect(item, SIGNAL(heightChanged()), this, SLOT(sizeChanged()));
            if (d->initialSize.height() <= 0 && d->qmlRoot->width() > 0)
                d->initialSize.setWidth(d->qmlRoot->width());
            if (d->initialSize.height() <= 0 && d->qmlRoot->height() > 0)
                d->initialSize.setHeight(d->qmlRoot->height());
            resize(d->initialSize);

            if (d->resizeMode == SizeRootObjectToView) {
                d->qmlRoot->setWidth(width());
                d->qmlRoot->setHeight(height());
            } else {
                QSize sz(d->qmlRoot->width(),d->qmlRoot->height());
                emit sceneResized(sz);
                resize(sz);
            }
            updateGeometry();
        } else if (QGraphicsObject *item = qobject_cast<QGraphicsObject *>(obj)) {
            d->scene.addItem(item);
            qWarning() << "QmlView::resizeMode is not honored for components of type QGraphicsObject";
        } else if (QWidget *wid = qobject_cast<QWidget *>(obj)) {
            window()->setAttribute(Qt::WA_OpaquePaintEvent, false);
            window()->setAttribute(Qt::WA_NoSystemBackground, false);
            if (!layout()) {
                setLayout(new QVBoxLayout);
                layout()->setContentsMargins(0, 0, 0, 0);
            } else if (layout()->count()) {
                // Hide the QGraphicsView in GV mode.
                QLayoutItem *item = layout()->itemAt(0);
                if (item->widget())
                    item->widget()->hide();
            }
            layout()->addWidget(wid);
            emit sceneResized(wid->size());
        }
    }
    emit statusChanged(status());
}

/*!
  \internal
 */
void QmlView::sizeChanged()
{
    // delay, so we catch both width and height changing.
    d->resizetimer.start(0,this);
}

/*!
  \internal
  If the \l {QTimerEvent} {timer event} \a e is this
  view's resize timer, sceneResized() is emitted.
 */
void QmlView::timerEvent(QTimerEvent* e)
{
    if (!e || e->timerId() == d->resizetimer.timerId()) {
        if (d->qmlRoot) {
            QSize sz(d->qmlRoot->width(),d->qmlRoot->height());
            emit sceneResized(sz);
            //if (!d->resizable)
                //resize(sz);
        }
        d->resizetimer.stop();
        updateGeometry();
    }
}

/*!
    \internal
    The size hint is the size of the root item.
*/
QSize QmlView::sizeHint() const
{
    if (d->qmlRoot) {
        if (d->initialSize.width() <= 0)
            d->initialSize.setWidth(d->qmlRoot->width());
        if (d->initialSize.height() <= 0)
            d->initialSize.setHeight(d->qmlRoot->height());
    }
    return d->initialSize;
}

/*!
  Returns the view's root \l {QGraphicsObject} {item}.
 */
QGraphicsObject *QmlView::rootObject() const
{
    return d->root;
}

/*!
  \internal
  This function handles the \l {QResizeEvent} {resize event}
  \a e.
 */
void QmlView::resizeEvent(QResizeEvent *e)
{
    if (d->resizeMode == SizeRootObjectToView && d->qmlRoot) {
        d->qmlRoot->setWidth(width());
        d->qmlRoot->setHeight(height());
    }
    if (d->qmlRoot) {
        setSceneRect(QRectF(0, 0, d->qmlRoot->width(), d->qmlRoot->height()));
    } else if (d->root) {
        setSceneRect(d->root->boundingRect());
    } else {
        setSceneRect(rect());
    }
    QGraphicsView::resizeEvent(e);
}

/*!
    \internal
*/
void QmlView::paintEvent(QPaintEvent *event)
{
    int time = 0;
    if (frameRateDebug() || QmlViewDebugServer::isDebuggingEnabled())
        time = d->frameTimer.restart();
    QGraphicsView::paintEvent(event);
    if (QmlViewDebugServer::isDebuggingEnabled())
        qfxViewDebugServer()->addTiming(d->frameTimer.elapsed(), time);
    if (frameRateDebug())
        qDebug() << "paintEvent:" << d->frameTimer.elapsed() << "time since last frame:" << time;
}

QT_END_NAMESPACE
