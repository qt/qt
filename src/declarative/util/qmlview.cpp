/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
    QmlViewPrivate(QmlView *w)
        : q(w), root(0), component(0), resizable(false) {}

    QmlView *q;
    QmlGraphicsItem *root;

    QUrl source;
    QString qml;

    QmlEngine engine;
    QmlComponent *component;
    QBasicTimer resizetimer;

    QSize initialSize;
    bool resizable;
    QTime frameTimer;

    void init();

    QGraphicsScene scene;
};

/*!
    \class QmlView
    \brief The QmlView class provides a widget for displaying a Qt Declarative user interface.

    QmlView currently provides a minimal interface for displaying QML
    files, and connecting between QML and C++ Qt objects.

    Typical usage:
    \code
    ...
    QmlView *view = new QmlView(this);
    vbox->addWidget(view);

    QUrl url(fileName);
    view->setUrl(url);
    ...
    view->execute();
    ...
    view->show();
    \endcode

    To receive errors related to loading and executing QML with QmlView,
    you can connect to the errors() signal.
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
  The destructor clears the view's \l {QmlGraphicsItem} {items} and
  deletes the internal representation.

  \sa clearItems()
 */
QmlView::~QmlView()
{
    clearItems();
    delete d; d = 0;
}

/*!
  Sets the source to the \a url. The QML string is set to
  empty.
 */
void QmlView::setUrl(const QUrl& url)
{
    d->source = url;
    d->qml = QString();
}

/*!
  Returns the source URL, if set.

  \sa setUrl()
 */
QUrl QmlView::url() const
{
    return d->source;
}

/*!
  Sets the source to the URL from the \a filename, and sets
  the QML string to \a qml.
 */
void QmlView::setQml(const QString &qml, const QString &filename)
{
    d->source = QUrl::fromLocalFile(filename);
    d->qml = qml;
}

/*!
  Returns the QML string.
 */
QString QmlView::qml() const
{
    return d->qml;
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
  Displays the Qt Declarative user interface.
*/
void QmlView::execute()
{
    if (d->qml.isEmpty()) {
        d->component = new QmlComponent(&d->engine, d->source, this);
    } else {
        d->component = new QmlComponent(&d->engine, this);
        d->component->setData(d->qml.toUtf8(), d->source);
    }
    connect (&d->engine, SIGNAL (quit ()), this, SIGNAL (quit ()));

    if (!d->component->isLoading()) {
        continueExecute();
    } else {
        connect(d->component, SIGNAL(statusChanged(QmlComponent::Status)), this, SLOT(continueExecute()));
    }
}


/*!
  \internal
 */
void QmlView::continueExecute()
{
    disconnect(d->component, SIGNAL(statusChanged(QmlComponent::Status)), this, SLOT(continueExecute()));

    if (!d->component) {
        qWarning() << "Error in loading" << d->source;
        return;
    }

    if(d->component->isError()) {
        QList<QmlError> errorList = d->component->errors();
        foreach (const QmlError &error, errorList) {
            qWarning() << error;
        }
        emit errors(errorList);

        return;
    }

    QObject *obj = d->component->create();

    if(d->component->isError()) {
        QList<QmlError> errorList = d->component->errors();
        foreach (const QmlError &error, errorList) {
            qWarning() << error;
        }
        emit errors(errorList);

        return;
    }

    if (obj) {
        if (QmlGraphicsItem *item = qobject_cast<QmlGraphicsItem *>(obj)) {

            d->scene.addItem(item);

            QPerformanceLog::displayData();
            QPerformanceLog::clear();
            d->root = item;
            connect(item, SIGNAL(widthChanged()), this, SLOT(sizeChanged()));
            connect(item, SIGNAL(heightChanged()), this, SLOT(sizeChanged()));
            if (d->initialSize.height() <= 0 && d->root->width() > 0)
                d->initialSize.setWidth(d->root->width());
            if (d->initialSize.height() <= 0 && d->root->height() > 0)
                d->initialSize.setHeight(d->root->height());
            resize(d->initialSize);

            if (d->resizable) {
                d->root->setWidth(width());
                d->root->setHeight(height());
            } else {
                QSize sz(d->root->width(),d->root->height());
                emit sceneResized(sz);
                resize(sz);
            }
            updateGeometry();
            emit initialSize(d->initialSize);
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
            emit initialSize(wid->size());
        }
    }
}

/*! \fn void QmlView::sceneResized(QSize size)
  This signal is emitted when the view is resized to \a size.
 */

/*! \fn void QmlView::initialSize(QSize size)
  This signal is emitted when the initial \a size of the root item is known.
 */

/*! \fn void QmlView::errors(const QList<QmlError> &errors)
  This signal is emitted when the qml loaded contains \a errors.
 */

/*!
  \internal
 */
void QmlView::sizeChanged()
{
    // delay, so we catch both width and height changing.
    d->resizetimer.start(0,this);
}

/*!
  If the \l {QTimerEvent} {timer event} \a e is this
  view's resize timer, sceneResized() is emitted.
 */
void QmlView::timerEvent(QTimerEvent* e)
{
    if (!e || e->timerId() == d->resizetimer.timerId()) {
        if (d->root) {
            QSize sz(d->root->width(),d->root->height());
            emit sceneResized(sz);
            //if (!d->resizable)
                //resize(sz);
        }
        d->resizetimer.stop();
        updateGeometry();
    }
}

// modelled on QScrollArea::widgetResizable
/*!
    \property QmlView::contentResizable
    \brief whether the view should resize the canvas contents

    If this property is set to false (the default), the view
    resizes with the root item in the QML.

    If this property is set to true, the view will
    automatically resize the root item.

    Regardless of this property, the sizeHint of the view
    is the initial size of the root item. Note though that
    since QML may load dynamically, that size may change.

    \sa initialSize()
*/

void QmlView::setContentResizable(bool on)
{
    if (d->resizable != on) {
        d->resizable = on;
        if (d->root) {
            if (on) {
                d->root->setWidth(width());
                d->root->setHeight(height());
            } else {
                d->root->setWidth(d->initialSize.width());
                d->root->setHeight(d->initialSize.height());
            }
        }
    }
}

bool QmlView::contentResizable() const
{
    return d->resizable;
}


/*!
    The size hint is the size of the root item.
*/
QSize QmlView::sizeHint() const
{
    if (d->root) {
        if (d->initialSize.width() <= 0)
            d->initialSize.setWidth(d->root->width());
        if (d->initialSize.height() <= 0)
            d->initialSize.setHeight(d->root->height());
    }
    return d->initialSize;
}

/*!
  Creates a \l{QmlComponent} {component} from the \a qml
  string, and returns it as an \l {QmlGraphicsItem} {item}. If the
  \a parent item is provided, it becomes the new item's
  parent. \a parent should be in this view's item hierarchy.
 */
QmlGraphicsItem* QmlView::addItem(const QString &qml, QmlGraphicsItem* parent)
{
    if (!d->root)
        return 0;

    QmlComponent component(&d->engine);
    component.setData(qml.toUtf8(), QUrl());
    if(d->component->isError()) {
        QList<QmlError> errorList = d->component->errors();
        foreach (const QmlError &error, errorList) {
            qWarning() << error;
        }
        emit errors(errorList);

        return 0;
    }

    QObject *obj = component.create();
    if(d->component->isError()) {
        QList<QmlError> errorList = d->component->errors();
        foreach (const QmlError &error, errorList) {
            qWarning() << error;
        }
        emit errors(errorList);

        return 0;
    }

    if (obj){
        QmlGraphicsItem *item = static_cast<QmlGraphicsItem *>(obj);
        if (!parent)
            parent = d->root;

        item->setParentItem(parent);
        return item;
    }
    return 0;
}

/*!
  Deletes the view's \l {QmlGraphicsItem} {items} and clears the \l {QmlEngine}
  {QML engine's} Component cache.
 */
void QmlView::reset()
{
    clearItems();
    d->engine.clearComponentCache();
    d->initialSize = QSize();
}

/*!
  Deletes the view's \l {QmlGraphicsItem} {items}.
 */
void QmlView::clearItems()
{
    if (!d->root)
        return;
    delete d->root;
    d->root = 0;
}

/*!
  Returns the view's root \l {QmlGraphicsItem} {item}.
 */
QmlGraphicsItem *QmlView::root() const
{
    return d->root;
}

/*!
  This function handles the \l {QResizeEvent} {resize event}
  \a e.
 */
void QmlView::resizeEvent(QResizeEvent *e)
{
    if (d->resizable && d->root) {
        d->root->setWidth(width());
        d->root->setHeight(height());
    }
    if (d->root) {
        setSceneRect(QRectF(0, 0, d->root->width(), d->root->height()));
    } else {
        setSceneRect(rect());
    }
    QGraphicsView::resizeEvent(e);
}

/*!
    \reimp
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
