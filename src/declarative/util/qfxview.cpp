/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qscriptvalueiterator.h"
#include "qdebug.h"
#include "qtimer.h"
#include "qevent.h"
#include "qdir.h"
#include "qcoreapplication.h"
#include "qfontdatabase.h"
#include "qicon.h"
#include "qurl.h"
#include "qboxlayout.h"
#include "qbasictimer.h"

#include "qml.h"
#include "qfxitem.h"
#include "private/qperformancelog_p.h"
#include "private/qfxperf_p.h"

#include "qfxview.h"
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcontext.h>
#include <QtDeclarative/qmldebug.h>

QT_BEGIN_NAMESPACE

DEFINE_BOOL_CONFIG_OPTION(frameRateDebug, QML_SHOW_FRAMERATE)

static QVariant stringToKeySequence(const QString &str)
{
    return QVariant::fromValue(QKeySequence(str));
}

class QFxViewPrivate
{
public:
    QFxViewPrivate(QFxView *w)
        : q(w), root(0), component(0), resizable(false) {}

    QFxView *q;
    QFxItem *root;

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
    \class QFxView
    \brief The QFxView class provides a widget for displaying a Qt Declarative user interface.

    QFxView currently provides a minimal interface for displaying QML
    files, and connecting between QML and C++ Qt objects.

    Typical usage:
    \code
    ...
    QFxView *view = new QFxView(this);
    vbox->addWidget(view);

    QUrl url(fileName);
    view->setUrl(url);
    ...
    view->execute();
    ...
    \endcode
*/

/*!
  \fn QFxView::QFxView(QWidget *parent)
  
  Constructs a QFxView with the given \a parent.
*/
QFxView::QFxView(QWidget *parent)
: QGraphicsView(parent), d(new QFxViewPrivate(this))
{
    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    d->init();
}

void QFxViewPrivate::init()
{
    // XXX: These need to be put in a central location for this kind of thing
    QmlMetaType::registerCustomStringConverter(QVariant::KeySequence, &stringToKeySequence);

#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::FontDatabase> perf;
#endif
    QFontDatabase database;

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
  The destructor clears the view's \l {QFxItem} {items} and
  deletes the internal representation.

  \sa clearItems()
 */
QFxView::~QFxView()
{
    clearItems();
    delete d; d = 0;
}

/*!
  Sets the source to the \a url. The QML string is set to
  empty.
 */
void QFxView::setUrl(const QUrl& url)
{
    d->source = url;
    d->qml = QString();
}

/*!
  Sets the source to the URL from the \a filename, and sets
  the QML string to \a qml.
 */
void QFxView::setQml(const QString &qml, const QString &filename)
{
    d->source = QUrl::fromLocalFile(filename);
    d->qml = qml;
}

/*!
  Returns the QML string.
 */
QString QFxView::qml() const
{
    return d->qml;
}

/*!
  Returns a pointer to the QmlEngine used for instantiating
  QML Components.
 */
QmlEngine* QFxView::engine()
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
QmlContext* QFxView::rootContext()
{
    return d->engine.rootContext();
}

/*!
  Displays the Qt Declarative user interface.
*/
void QFxView::execute()
{
    if (d->qml.isEmpty()) {
        d->component = new QmlComponent(&d->engine, d->source, this);
    } else {
        d->component = new QmlComponent(&d->engine, d->qml.toUtf8(), d->source);
    }

    if (!d->component->isLoading()) {
        continueExecute();
    } else {
        connect(d->component, SIGNAL(statusChanged(QmlComponent::Status)), this, SLOT(continueExecute()));
    }
}


/*!
  \internal
 */
void QFxView::continueExecute()
{
    disconnect(d->component, SIGNAL(statusChanged(QmlComponent::Status)), this, SLOT(continueExecute()));

    if (!d->component) {
        qWarning() << "Error in loading" << d->source;
        return;
    }

    if(d->component->isError()) {
        QList<QmlError> errorList = d->component->errors();
        emit errors(errorList);
        foreach (const QmlError &error, errorList) {
            qWarning() << error;
        }

        return;
    }

    QObject *obj = d->component->create();

    if(d->component->isError()) {
        QList<QmlError> errorList = d->component->errors();
        emit errors(errorList);
        foreach (const QmlError &error, errorList) {
            qWarning() << error;
        }

        return;
    }

    if (obj) {
        if (QFxItem *item = qobject_cast<QFxItem *>(obj)) {

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
            if (d->resizable) {
                d->root->setWidth(width());
                d->root->setHeight(height());
            } else {
                QSize sz(d->root->width(),d->root->height());
                emit sceneResized(sz);
                resize(sz);
            }
        } else if (QWidget *wid = qobject_cast<QWidget *>(obj)) {
            window()->setAttribute(Qt::WA_OpaquePaintEvent, false);
            window()->setAttribute(Qt::WA_NoSystemBackground, false);
            if (!layout()) {
                setLayout(new QVBoxLayout);
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
}

/*! \fn void QFxView::sceneResized(QSize size)
  This signal is emitted when the view is resized to \a size.
 */

/*! \fn void QFxView::error(const QList<QmlError> &errors)
  This signal is emitted when the qml loaded contains errors.
 */

/*!
  \internal
 */
void QFxView::sizeChanged()
{
    // delay, so we catch both width and height changing.
    d->resizetimer.start(0,this);
}

/*!
  If the \l {QTimerEvent} {timer event} \a e is this
  view's resize timer, sceneResized() is emitted.
 */
void QFxView::timerEvent(QTimerEvent* e)
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
    \property QFxView::contentResizable
    \brief whether the view should resize the canvas contents

    If this property is set to false (the default), the view
    resizes with the root item in the QML.

    If this property is set to true, the view will
    automatically resize the root item.

    Regardless of this property, the sizeHint of the view
    is the initial size of the root item.
*/

void QFxView::setContentResizable(bool on)
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

bool QFxView::contentResizable() const
{
    return d->resizable;
}


/*!
    The size hint is the size of the root item.
*/
QSize QFxView::sizeHint() const
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
  string, and returns it as an \l {QFxItem} {item}. If the
  \a parent item is provided, it becomes the new item's
  parent. \a parent should be in this view's item hierarchy.
 */
QFxItem* QFxView::addItem(const QString &qml, QFxItem* parent)
{
    if (!d->root)
        return 0;

    QmlComponent component(&d->engine, qml.toUtf8(), QUrl());
    if(d->component->isError()) {
        QList<QmlError> errorList = d->component->errors();
        emit errors(errorList);
        foreach (const QmlError &error, errorList) {
            qWarning() << error;
        }

        return 0;
    }

    QObject *obj = component.create();
    if(d->component->isError()) {
        QList<QmlError> errorList = d->component->errors();
        emit errors(errorList);
        foreach (const QmlError &error, errorList) {
            qWarning() << error;
        }

        return 0;
    }

    if (obj){
        QFxItem *item = static_cast<QFxItem *>(obj);
        if (!parent)
            parent = d->root;

        item->setParentItem(parent);
        return item;
    }
    return 0;
}

/*!
  Deletes the view's \l {QFxItem} {items} and the \l {QmlEngine}
  {QML engine's} Component cache.
 */
void QFxView::reset()
{
    clearItems();
    d->engine.clearComponentCache();
    d->initialSize = QSize();
}

/*!
  Deletes the view's \l {QFxItem} {items}.
 */
void QFxView::clearItems()
{
    if (!d->root)
        return;
    delete d->root;
    d->root = 0;
}

/*!
  Returns the view's root \l {QFxItem} {item}.
 */
QFxItem *QFxView::root() const
{
    return d->root;
}

/*!
  This function handles the \l {QResizeEvent} {resize event}
  \a e.
 */
void QFxView::resizeEvent(QResizeEvent *e)
{
    if (d->resizable && d->root) {
        d->root->setWidth(width());
        d->root->setHeight(height());
    }
    setSceneRect(rect());
    QGraphicsView::resizeEvent(e);
}

/*!
    \reimp
*/
void QFxView::paintEvent(QPaintEvent *event)
{
    int time = 0;
    if (frameRateDebug())
        time = d->frameTimer.restart();
    QGraphicsView::paintEvent(event);
    if (frameRateDebug())
        qDebug() << "paintEvent:" << d->frameTimer.elapsed() << "time since last frame:" << time;
}

/*! \fn void QFxView::focusInEvent(QFocusEvent *e)
  This virtual function does nothing with the event \a e
  in this class.
 */
void QFxView::focusInEvent(QFocusEvent *)
{
    // Do nothing (do not call QWidget::update())
}


/*! \fn void QFxView::focusOutEvent(QFocusEvent *e)
  This virtual function does nothing with the event \a e
  in this class.
 */
void QFxView::focusOutEvent(QFocusEvent *)
{
    // Do nothing (do not call QWidget::update())
}

QT_END_NAMESPACE
