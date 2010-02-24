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

#include "qdeclarativegraphicswidget.h"

#include "qperformancelog_p_p.h"
#include "qfxperf_p_p.h"

#include <qdeclarative.h>
#include <qdeclarativeitem.h>
#include <qdeclarativeengine.h>
#include <qdeclarativecontext.h>
#include <qdeclarativedebug_p.h>
#include <qdeclarativedebugservice_p.h>
#include <qdeclarativeglobal_p.h>
#include <qdeclarativeerror.h>

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
#include <private/qguard_p.h>

QT_BEGIN_NAMESPACE

class QDeclarativeGraphicsWidgetSharedQDeclarativeEngine
{
public:
    QDeclarativeGraphicsWidgetSharedQDeclarativeEngine(){}

    static QDeclarativeEngine* sharedEngine(){
        if(!references)
            return 0;
        return &QDeclarativeGraphicsWidgetSharedQDeclarativeEngine::m_instance->engine;
    }
    static void attach(){
        if(!references++)
            m_instance = new QDeclarativeGraphicsWidgetSharedQDeclarativeEngine();
    }

    static void detach(){
        if(!--references)
            delete m_instance;
    }

private:
    static QDeclarativeGraphicsWidgetSharedQDeclarativeEngine* m_instance;
    static int references;
    QDeclarativeEngine engine;
};

int QDeclarativeGraphicsWidgetSharedQDeclarativeEngine::references = 0;
QDeclarativeGraphicsWidgetSharedQDeclarativeEngine* QDeclarativeGraphicsWidgetSharedQDeclarativeEngine::m_instance = 0;

class QDeclarativeGraphicsWidgetPrivate
{
public:
    QDeclarativeGraphicsWidgetPrivate()
        : root(0), component(0),
          resizeMode(QDeclarativeGraphicsWidget::SizeViewToRootObject) 
    {
        QDeclarativeGraphicsWidgetSharedQDeclarativeEngine::attach();
        engine = QDeclarativeGraphicsWidgetSharedQDeclarativeEngine::sharedEngine();
    }

    ~QDeclarativeGraphicsWidgetPrivate()
    {
        QDeclarativeGraphicsWidgetSharedQDeclarativeEngine::detach();
    }

    QGuard<QGraphicsObject> root;
    QGuard<QDeclarativeItem> declarativeRoot;

    QUrl source;

    QDeclarativeEngine* engine;
    QDeclarativeComponent *component;
    QBasicTimer resizetimer;

    mutable QSize initialSize;
    QDeclarativeGraphicsWidget::ResizeMode resizeMode;

    void init();

};

/*!
    \class QDeclarativeGraphicsWidget
    \brief The QDeclarativeGraphicsWidget class provides a QGraphicsWidget for displaying a Qt Declarative user interface.

    Any QGraphicsObject or QDeclarativeGraphicsItem
    created via Qt Declarative can be placed on a standard QGraphicsScene and viewed with a standard
    QGraphicsView.

    QDeclarativeGraphicsWidget is a convenience class which handles QDeclarativeComponent loading and object creation.
    It shares the same QDeclarativeEngine between all QDeclarativeGraphicsWidgets in the application, to minimize overhead.

    QDeclarativeGraphicsWidget is ideal for when you have many small components styled with Qt Declarative, and want to integrate
    them into a larger QGraphicsItem based scene. If your interface is primarily Qt Declarative based, consider using QDeclarativeView
    instead.

    Note that the following settings on your QGraphicsScene/View are recommended for optimal performance with Qt Declarative:
        \list
        \o QGraphicsView::setOptimizationFlags(QGraphicsView::DontSavePainterState);
        \o QGraphicsView::setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
        \o QGraphicsScene::setItemIndexMethod(QGraphicsScene::NoIndex);
        \endlist
    Also note that the following settings on your QGraphicsScene/View are required for key handling in Qt Declarative to work:
        \list
        \o QGraphicsView::viewport()->setFocusPolicy(Qt::NoFocus);
        \o QGraphicsScene::setStickyFocus(true);
        \endlist

    To receive errors related to loading and executing declarative files with QDeclarativeGraphicsWidget,
    you can connect to the statusChanged() signal and monitor for QDeclarativeGraphicsWidget::Error.
    The errors are available via QDeclarativeGraphicsWidget::errors().
*/


/*! \fn void QDeclarativeGraphicsWidget::sceneResized(QSize size)
  This signal is emitted when the view is resized to \a size.
*/

/*! \fn void QDeclarativeGraphicsWidget::statusChanged(QDeclarativeGraphicsWidget::Status status)
    This signal is emitted when the component's current \l{QDeclarativeGraphicsWidget::Status} {status} changes.
*/

/*!
  \fn QDeclarativeGraphicsWidget::QDeclarativeGraphicsWidget(QGraphicsItem *parent)

  Constructs a QDeclarativeGraphicsWidget with the given \a parent.
*/
QDeclarativeGraphicsWidget::QDeclarativeGraphicsWidget(QGraphicsItem *parent)
: QGraphicsWidget(parent), d(new QDeclarativeGraphicsWidgetPrivate)
{
    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    d->init();
}

void QDeclarativeGraphicsWidgetPrivate::init()
{
}

/*!
  The destructor clears the view's \l {QGraphicsObject} {items} and
  deletes the internal representation.
 */
QDeclarativeGraphicsWidget::~QDeclarativeGraphicsWidget()
{
    delete d->root;
    delete d;
}

/*!
    Sets the source to the \a url.

    Will also load the QML file and instantiate the component.

 */
void QDeclarativeGraphicsWidget::setSource(const QUrl& url)
{
    d->source = url;

    //Execute
    if(d->root)
        delete d->root;
    if(d->component)
        delete d->component;
    d->component = new QDeclarativeComponent(d->engine, d->source, this);

    if (!d->component->isLoading()) {
        continueExecute();
    } else {
        connect(d->component, SIGNAL(statusChanged(QDeclarativeComponent::Status)), this, SLOT(continueExecute()));
    }
}

/*!
  Returns the source URL, if set.

  \sa setSource()
 */
QUrl QDeclarativeGraphicsWidget::source() const
{
    return d->source;
}

/*!
  Returns a pointer to the QDeclarativeEngine used for instantiating
  Qt Declarative Components.
 */
QDeclarativeEngine* QDeclarativeGraphicsWidget::engine()
{
    return d->engine;
}

/*!
  This function returns the root of the context hierarchy.  Each declarative
  component is instantiated in a QDeclarativeContext.  QDeclarativeContext's are
  essential for passing data to declarative components.  In Qt Declarative, contexts are
  arranged hierarchically and this hierarchy is managed by the
  QDeclarativeEngine.
 */
QDeclarativeContext* QDeclarativeGraphicsWidget::rootContext()
{
    return d->engine->rootContext();
}


/*!
  \enum QDeclarativeGraphicsWidget::Status

    Specifies the loading status of the QDeclarativeGraphicsWidget.

    \value Null This QDeclarativeGraphicsWidget has no source set.
    \value Ready This QDeclarativeGraphicsWidget has loaded and created the declarative component.
    \value Loading This QDeclarativeGraphicsWidget is loading network data.
    \value Error An error has occured.  Calling errorDescription() to retrieve a description.
*/

/*!
    \property QDeclarativeGraphicsWidget::status
    The component's current \l{QDeclarativeGraphicsWidget::Status} {status}.
*/

QDeclarativeGraphicsWidget::Status QDeclarativeGraphicsWidget::status() const
{
    if (!d->component)
        return QDeclarativeGraphicsWidget::Null;

    return QDeclarativeGraphicsWidget::Status(d->component->status());
}

/*!
    Return the list of errors that occured during the last compile or create
    operation.  An empty list is returned if isError() is not set.
*/
QList<QDeclarativeError> QDeclarativeGraphicsWidget::errors() const
{
    if (d->component)
        return d->component->errors();
    return QList<QDeclarativeError>();
}


/*!
    \property QDeclarativeGraphicsWidget::resizeMode
    \brief whether the view should resize the canvas contents

    If this property is set to SizeViewToRootObject (the default), the view
    resizes with the root item in the declarative file.

    If this property is set to SizeRootObjectToView, the view will
    automatically resize the root item.

    Regardless of this property, the sizeHint of the view
    is the initial size of the root item. Note though that
    since declarative files may load dynamically, that size may change.

    \sa initialSize()
*/

void QDeclarativeGraphicsWidget::setResizeMode(ResizeMode mode)
{
    if (d->resizeMode == mode)
        return;

    d->resizeMode = mode;
    if (d->declarativeRoot) {
        if (d->resizeMode == SizeRootObjectToView) {
            d->declarativeRoot->setWidth(size().width());
            d->declarativeRoot->setHeight(size().height());
        } else {
            d->declarativeRoot->setWidth(d->initialSize.width());
            d->declarativeRoot->setHeight(d->initialSize.height());
        }
    }
}

QDeclarativeGraphicsWidget::ResizeMode QDeclarativeGraphicsWidget::resizeMode() const
{
    return d->resizeMode;
}

/*!
  \internal
 */
void QDeclarativeGraphicsWidget::continueExecute()
{

    disconnect(d->component, SIGNAL(statusChanged(QDeclarativeComponent::Status)), this, SLOT(continueExecute()));

    if (d->component->isError()) {
        QList<QDeclarativeError> errorList = d->component->errors();
        foreach (const QDeclarativeError &error, errorList) {
            qWarning() << error;
        }
        emit statusChanged(status());
        return;
    }

    QObject *obj = d->component->create();

    if(d->component->isError()) {
        QList<QDeclarativeError> errorList = d->component->errors();
        foreach (const QDeclarativeError &error, errorList) {
            qWarning() << error;
        }
        emit statusChanged(status());
        return;
    }

    if (obj) {
        if (QDeclarativeItem *item = qobject_cast<QDeclarativeItem *>(obj)) {

            item->QGraphicsItem::setParentItem(this);
            item->QObject::setParent(this);

            d->root = item;
            d->declarativeRoot = item;
            connect(item, SIGNAL(widthChanged()), this, SLOT(sizeChanged()));
            connect(item, SIGNAL(heightChanged()), this, SLOT(sizeChanged()));
            if (d->initialSize.height() <= 0 && d->declarativeRoot->width() > 0)
                d->initialSize.setWidth(d->declarativeRoot->width());
            if (d->initialSize.height() <= 0 && d->declarativeRoot->height() > 0)
                d->initialSize.setHeight(d->declarativeRoot->height());
            resize(d->initialSize);

            if (d->resizeMode == SizeRootObjectToView) {
                d->declarativeRoot->setWidth(size().width());
                d->declarativeRoot->setHeight(size().height());
            } else {
                QSizeF sz(d->declarativeRoot->width(),d->declarativeRoot->height());
                emit sceneResized(sz);
                resize(sz);
            }
            updateGeometry();
        } else if (QGraphicsObject *item = qobject_cast<QGraphicsObject *>(obj)) {
            item->setParent(this);
            qWarning() << "QDeclarativeGraphicsWidget::resizeMode is not honored for components of type QGraphicsObject";
        } else if (qobject_cast<QWidget *>(obj)) {
            qWarning() << "QDeclarativeGraphicsWidget does not support loading QML files containing QWidgets";
        }
    }
    emit statusChanged(status());
}

/*!
  \internal
 */
void QDeclarativeGraphicsWidget::sizeChanged()
{
    // delay, so we catch both width and height changing.
    d->resizetimer.start(0,this);
}

/*!
  \internal
  If the \l {QTimerEvent} {timer event} \a e is this
  view's resize timer, sceneResized() is emitted.
 */
void QDeclarativeGraphicsWidget::timerEvent(QTimerEvent* e)
{
    if (!e || e->timerId() == d->resizetimer.timerId()) {
        if (d->declarativeRoot) {
            QSize sz(d->declarativeRoot->width(),d->declarativeRoot->height());
            emit sceneResized(sz);
        }
        d->resizetimer.stop();
        updateGeometry();
    }
}

/*!
    \internal
    The size hint is the size of the root item.
*/
QSizeF QDeclarativeGraphicsWidget::sizeHint() const
{
    if (d->declarativeRoot) {
        if (d->initialSize.width() <= 0)
            d->initialSize.setWidth(d->declarativeRoot->width());
        if (d->initialSize.height() <= 0)
            d->initialSize.setHeight(d->declarativeRoot->height());
    }
    return d->initialSize;
}

/*!
  Returns the view's root \l {QGraphicsObject} {item}.
 */
QGraphicsObject *QDeclarativeGraphicsWidget::rootObject() const
{
    return d->root;
}

/*!
  \internal
  This function handles the \l {QGraphicsSceneResizeEvent} {resize event}
  \a e.
 */
void QDeclarativeGraphicsWidget::resizeEvent(QGraphicsSceneResizeEvent *e)
{
    if (d->resizeMode == SizeRootObjectToView && d->declarativeRoot) {
        d->declarativeRoot->setWidth(size().width());
        d->declarativeRoot->setHeight(size().height());
    }
    QGraphicsWidget::resizeEvent(e);
}

QT_END_NAMESPACE
