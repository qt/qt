// Commit: 55c4d94dfea78951f3371d3697a3cb28539b3012
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

#include "qsgview.h"

#include "qsgcanvas_p.h"
#include "qsgitem_p.h"
#include "qsgitemchangelistener_p.h"

#include <private/qdeclarativedebugtrace_p.h>

#include <QtDeclarative/qdeclarativeengine.h>
#include <private/qdeclarativeengine_p.h>
#include <QtCore/qbasictimer.h>

// XXX todo - This whole class should probably be merged with QDeclarativeView for 
// maximum seamlessness
QT_BEGIN_NAMESPACE

DEFINE_BOOL_CONFIG_OPTION(frameRateDebug, QML_SHOW_FRAMERATE)

class QSGViewPrivate : public QSGCanvasPrivate, 
                       public QSGItemChangeListener
{
    Q_DECLARE_PUBLIC(QSGView)
public:
    QSGViewPrivate();
    ~QSGViewPrivate();

    void execute();
    void itemGeometryChanged(QSGItem *item, const QRectF &newGeometry, const QRectF &oldGeometry);
    void initResize();
    void updateSize();
    void setRootObject(QObject *);

    void init();

    QSize rootObjectSize() const;

    QPointer<QSGItem> root;

    QUrl source;

    QDeclarativeEngine engine;
    QDeclarativeComponent *component;
    QBasicTimer resizetimer;

    QSGView::ResizeMode resizeMode;
    QSize initialSize;
    QElapsedTimer frameTimer;
};

void QSGViewPrivate::init()
{
    q_func()->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    QDeclarativeEnginePrivate::get(&engine)->sgContext = QSGCanvasPrivate::context;
}

QSGViewPrivate::QSGViewPrivate()
: root(0), component(0), resizeMode(QSGView::SizeViewToRootObject), initialSize(0,0) 
{
}

QSGViewPrivate::~QSGViewPrivate() 
{ 
    delete root; 
}

void QSGViewPrivate::execute()
{
    Q_Q(QSGView);
    if (root) {
        delete root;
        root = 0;
    }
    if (component) {
        delete component;
        component = 0;
    }
    if (!source.isEmpty()) {
        component = new QDeclarativeComponent(&engine, source, q);
        if (!component->isLoading()) {
            q->continueExecute();
        } else {
            QObject::connect(component, SIGNAL(statusChanged(QDeclarativeComponent::Status)), 
                             q, SLOT(continueExecute()));
        }
    }
}

void QSGViewPrivate::itemGeometryChanged(QSGItem *resizeItem, const QRectF &newGeometry, const QRectF &oldGeometry)
{
    Q_Q(QSGView);
    if (resizeItem == root && resizeMode == QSGView::SizeViewToRootObject) {
        // wait for both width and height to be changed
        resizetimer.start(0,q);
    }
    QSGItemChangeListener::itemGeometryChanged(resizeItem, newGeometry, oldGeometry);
}

QSGView::QSGView(QWidget *parent, Qt::WindowFlags f)
: QSGCanvas(*(new QSGViewPrivate), parent, f)
{
    d_func()->init();
}

QSGView::QSGView(const QGLFormat &format, QWidget *parent, Qt::WindowFlags f)
: QSGCanvas(*(new QSGViewPrivate), format, parent, f)
{
    d_func()->init();
}

QSGView::QSGView(const QUrl &source, QWidget *parent, Qt::WindowFlags f)
: QSGCanvas(*(new QSGViewPrivate), parent, f)
{
    d_func()->init();
    setSource(source);
}

QSGView::QSGView(const QUrl &source, const QGLFormat &format, QWidget *parent, Qt::WindowFlags f)
: QSGCanvas(*(new QSGViewPrivate), format, parent, f)
{
    d_func()->init();
    setSource(source);
}

QSGView::~QSGView()
{
}

void QSGView::setSource(const QUrl& url)
{
    Q_D(QSGView);
    d->source = url;
    d->execute();
}

QUrl QSGView::source() const
{
    Q_D(const QSGView);
    return d->source;
}

QDeclarativeEngine* QSGView::engine() const
{
    Q_D(const QSGView);
    return const_cast<QDeclarativeEngine *>(&d->engine);
}

QDeclarativeContext* QSGView::rootContext() const
{
    Q_D(const QSGView);
    return d->engine.rootContext();
}

QSGView::Status QSGView::status() const
{
    Q_D(const QSGView);
    if (!d->component)
        return QSGView::Null;

    return QSGView::Status(d->component->status());
}

QList<QDeclarativeError> QSGView::errors() const
{
    Q_D(const QSGView);
    if (d->component)
        return d->component->errors();
    return QList<QDeclarativeError>();
}

void QSGView::setResizeMode(ResizeMode mode)
{
    Q_D(QSGView);
    if (d->resizeMode == mode)
        return;

    if (d->root) {
        if (d->resizeMode == SizeViewToRootObject) {
            QSGItemPrivate *p = QSGItemPrivate::get(d->root);
            p->removeItemChangeListener(d, QSGItemPrivate::Geometry);
        }
    }

    d->resizeMode = mode;
    if (d->root) {
        d->initResize();
    }
}

void QSGViewPrivate::initResize()
{
    if (root) {
        if (resizeMode == QSGView::SizeViewToRootObject) {
            QSGItemPrivate *p = QSGItemPrivate::get(root);
            p->addItemChangeListener(this, QSGItemPrivate::Geometry);
        }
    }
    updateSize();
}

void QSGViewPrivate::updateSize()
{
    Q_Q(QSGView);
    if (!root)
        return;

    if (resizeMode == QSGView::SizeViewToRootObject) {
        QSize newSize = QSize(root->width(), root->height());
        if (newSize.isValid() && newSize != q->size()) {
            q->resize(newSize);
        }
    } else if (resizeMode == QSGView::SizeRootObjectToView) {
        if (!qFuzzyCompare(q->width(), root->width()))
            root->setWidth(q->width());
        if (!qFuzzyCompare(q->height(), root->height()))
            root->setHeight(q->height());
    }

    q->updateGeometry();
}

QSize QSGViewPrivate::rootObjectSize() const
{
    QSize rootObjectSize(0,0);
    int widthCandidate = -1;
    int heightCandidate = -1;
    if (root) {
        widthCandidate = root->width();
        heightCandidate = root->height();
    }
    if (widthCandidate > 0) {
        rootObjectSize.setWidth(widthCandidate);
    }
    if (heightCandidate > 0) {
        rootObjectSize.setHeight(heightCandidate);
    }
    return rootObjectSize;
}

QSGView::ResizeMode QSGView::resizeMode() const
{
    Q_D(const QSGView);
    return d->resizeMode;
}

/*!
  \internal
 */
void QSGView::continueExecute()
{
    Q_D(QSGView);
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

    d->setRootObject(obj);
    emit statusChanged(status());
}


/*!
  \internal
*/
void QSGViewPrivate::setRootObject(QObject *obj)
{
    Q_Q(QSGView);
    if (root == obj)
        return;
    if (QSGItem *sgItem = qobject_cast<QSGItem *>(obj)) {
        root = sgItem;
        sgItem->setParentItem(q->QSGCanvas::rootItem());
    } else {
        qWarning() << "QSGView only supports loading of root objects that derive from QSGItem." << endl
                   << endl
                   << "If your example is using QML 2, (such as qmlscene) and the .qml file you" << endl
                   << "loaded has 'import QtQuick 1.0' or 'import Qt 4.7', this error will occur." << endl
                   << endl
                   << "To load files with 'import QtQuick 1.0' with QML 2, specify:" << endl
                   << "  QMLSCENE_IMPORT_NAME=quick1" << endl
                   << "on as an environment variable prior to launching the application." << endl
                   << endl
                   << "To load files with 'import Qt 4.7' with QML 2, specify:" << endl
                   << "  QMLSCENE_IMPORT_NAME=qt" << endl
                   << "on as an environment variable prior to launching the application." << endl;
        delete obj;
        root = 0;
    }

    if (root) {
        initialSize = rootObjectSize();
        if (initialSize != q->size()) {
            if (!(q->parentWidget() && q->parentWidget()->layout())) {
                q->resize(initialSize);
            }
        }
        initResize();
    }
}

/*!
  \internal
  If the \l {QTimerEvent} {timer event} \a e is this
  view's resize timer, sceneResized() is emitted.
 */
void QSGView::timerEvent(QTimerEvent* e)
{
    Q_D(QSGView);
    if (!e || e->timerId() == d->resizetimer.timerId()) {
        d->updateSize();
        d->resizetimer.stop();
    }
}

/*!
    \internal
    Preferred size follows the root object geometry.
*/
QSize QSGView::sizeHint() const
{
    Q_D(const QSGView);
    QSize rootObjectSize = d->rootObjectSize();
    if (rootObjectSize.isEmpty()) {
        return size();
    } else {
        return rootObjectSize;
    }
}

QSize QSGView::initialSize() const
{
    Q_D(const QSGView);
    return d->initialSize;
}

QSGItem *QSGView::rootObject() const
{
    Q_D(const QSGView);
    return d->root;
}

/*!
  \internal
  This function handles the \l {QResizeEvent} {resize event}
  \a e.
 */
void QSGView::resizeEvent(QResizeEvent *e)
{
    Q_D(QSGView);
    if (d->resizeMode == SizeRootObjectToView) 
        d->updateSize();
    
    QSGCanvas::resizeEvent(e);
}

/*!
    \internal
*/
void QSGView::paintEvent(QPaintEvent *event)
{
    Q_D(QSGView);
    int time = 0;
    if (frameRateDebug()) 
        time = d->frameTimer.restart();

    QSGCanvas::paintEvent(event);

    if (frameRateDebug())
        qDebug() << "paintEvent:" << d->frameTimer.elapsed() << "time since last frame:" << time;
}

void QSGView::keyPressEvent(QKeyEvent *e)
{
    QDeclarativeDebugTrace::addEvent(QDeclarativeDebugTrace::Key);

    QSGCanvas::keyPressEvent(e);
}

void QSGView::keyReleaseEvent(QKeyEvent *e)
{
    QDeclarativeDebugTrace::addEvent(QDeclarativeDebugTrace::Key);

    QSGCanvas::keyReleaseEvent(e);
}

void QSGView::mouseMoveEvent(QMouseEvent *e)
{
    QDeclarativeDebugTrace::addEvent(QDeclarativeDebugTrace::Mouse);

    QSGCanvas::mouseMoveEvent(e);
}

void QSGView::mousePressEvent(QMouseEvent *e)
{
    QDeclarativeDebugTrace::addEvent(QDeclarativeDebugTrace::Mouse);

    QSGCanvas::mousePressEvent(e);
}

void QSGView::mouseReleaseEvent(QMouseEvent *e)
{
    QDeclarativeDebugTrace::addEvent(QDeclarativeDebugTrace::Mouse);

    QSGCanvas::mouseReleaseEvent(e);
}


QT_END_NAMESPACE
