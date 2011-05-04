// Commit: c44be8c0b27756a2025ebad1945632f3f7e4bebc
/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qsgitem.h"

#include "qsgcanvas.h"
#include <QtScript/qscriptengine.h>
#include "qsgcanvas_p.h"

#include "qsgevents_p_p.h"

#include <QtDeclarative/qdeclarativeitem.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativeview.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qdeclarativeinfo.h>
#include <QtGui/qgraphicstransform.h>
#include <QtGui/qpen.h>
#include <QtGui/qinputcontext.h>
#include <QtCore/qdebug.h>
#include <QtCore/qcoreevent.h>
#include <QtCore/qnumeric.h>

#include <private/qdeclarativeengine_p.h>
#include <private/qdeclarativestategroup_p.h>
#include <private/qdeclarativeopenmetaobject_p.h>
#include <private/qdeclarativestate_p.h>
#include <private/qlistmodelinterface_p.h>
#include <private/qsgitem_p.h>

#include <float.h>

// XXX todo Readd parentNotifier for faster parent bindings
// XXX todo Check that elements that create items handle memory correctly after visual ownership change

QT_BEGIN_NAMESPACE

QSGTransformPrivate::QSGTransformPrivate()
{
}

QSGTransform::QSGTransform(QObject *parent)
: QObject(*(new QSGTransformPrivate), parent)
{
}

QSGTransform::QSGTransform(QSGTransformPrivate &dd, QObject *parent)
: QObject(dd, parent)
{
}

QSGTransform::~QSGTransform()
{
    Q_D(QSGTransform);
    for (int ii = 0; ii < d->items.count(); ++ii) {
        QSGItemPrivate *p = QSGItemPrivate::get(d->items.at(ii));
        p->transforms.removeOne(this);
        p->dirty(QSGItemPrivate::Transform);
    }
}

void QSGTransform::update()
{
    Q_D(QSGTransform);
    for (int ii = 0; ii < d->items.count(); ++ii) {
        QSGItemPrivate *p = QSGItemPrivate::get(d->items.at(ii));
        p->dirty(QSGItemPrivate::Transform);
    }
}

QSGContents::QSGContents(QSGItem *item) 
: m_item(item), m_x(0), m_y(0), m_width(0), m_height(0)
{
    //### optimize
    connect(this, SIGNAL(rectChanged(QRectF)), m_item, SIGNAL(childrenRectChanged(QRectF)));
}

QSGContents::~QSGContents()
{
    QList<QSGItem *> children = m_item->childItems();
    for (int i = 0; i < children.count(); ++i) {
        QSGItem *child = children.at(i);
        QSGItemPrivate::get(child)->removeItemChangeListener(this, QSGItemPrivate::Geometry | QSGItemPrivate::Destroyed);
    }
}

QRectF QSGContents::rectF() const
{
    return QRectF(m_x, m_y, m_width, m_height);
}

void QSGContents::calcHeight(QSGItem *changed)
{
    qreal oldy = m_y;
    qreal oldheight = m_height;

    if (changed) {
        qreal top = oldy;
        qreal bottom = oldy + oldheight;
        qreal y = changed->y();
        if (y + changed->height() > bottom)
            bottom = y + changed->height();
        if (y < top)
            top = y;
        m_y = top;
        m_height = bottom - top;
    } else {
        qreal top = FLT_MAX;
        qreal bottom = 0;
        QList<QSGItem *> children = m_item->childItems();
        for (int i = 0; i < children.count(); ++i) {
            QSGItem *child = children.at(i);
            qreal y = child->y();
            if (y + child->height() > bottom)
                bottom = y + child->height();
            if (y < top)
                top = y;
        }
        if (!children.isEmpty())
            m_y = top;
        m_height = qMax(bottom - top, qreal(0.0));
    }

    if (m_height != oldheight || m_y != oldy)
        emit rectChanged(rectF());
}

void QSGContents::calcWidth(QSGItem *changed)
{
    qreal oldx = m_x;
    qreal oldwidth = m_width;

    if (changed) {
        qreal left = oldx;
        qreal right = oldx + oldwidth;
        qreal x = changed->x();
        if (x + changed->width() > right)
            right = x + changed->width();
        if (x < left)
            left = x;
        m_x = left;
        m_width = right - left;
    } else {
        qreal left = FLT_MAX;
        qreal right = 0;
        QList<QSGItem *> children = m_item->childItems();
        for (int i = 0; i < children.count(); ++i) {
            QSGItem *child = children.at(i);
            qreal x = child->x();
            if (x + child->width() > right)
                right = x + child->width();
            if (x < left)
                left = x;
        }
        if (!children.isEmpty())
            m_x = left;
        m_width = qMax(right - left, qreal(0.0));
    }

    if (m_width != oldwidth || m_x != oldx)
        emit rectChanged(rectF());
}

void QSGContents::complete()
{
    QList<QSGItem *> children = m_item->childItems();
    for (int i = 0; i < children.count(); ++i) {
        QSGItem *child = children.at(i);
        QSGItemPrivate::get(child)->addItemChangeListener(this, QSGItemPrivate::Geometry | QSGItemPrivate::Destroyed);
        //###what about changes to visibility?
    }

    calcGeometry();
}

void QSGContents::itemGeometryChanged(QSGItem *changed, const QRectF &newGeometry, const QRectF &oldGeometry)
{
    Q_UNUSED(changed)
    //### we can only pass changed if the left edge has moved left, or the right edge has moved right
    if (newGeometry.width() != oldGeometry.width() || newGeometry.x() != oldGeometry.x())
        calcWidth(/*changed*/);
    if (newGeometry.height() != oldGeometry.height() || newGeometry.y() != oldGeometry.y())
        calcHeight(/*changed*/);
}

void QSGContents::itemDestroyed(QSGItem *item)
{
    if (item)
        QSGItemPrivate::get(item)->removeItemChangeListener(this, QSGItemPrivate::Geometry | QSGItemPrivate::Destroyed);
    calcGeometry();
}

void QSGContents::childRemoved(QSGItem *item)
{
    if (item)
        QSGItemPrivate::get(item)->removeItemChangeListener(this, QSGItemPrivate::Geometry | QSGItemPrivate::Destroyed);
    calcGeometry();
}

void QSGContents::childAdded(QSGItem *item)
{
    if (item)
        QSGItemPrivate::get(item)->addItemChangeListener(this, QSGItemPrivate::Geometry | QSGItemPrivate::Destroyed);
    calcWidth(item);
    calcHeight(item);
}

QSGItemKeyFilter::QSGItemKeyFilter(QSGItem *item)
: m_processPost(false), m_next(0)
{
    QSGItemPrivate *p = item?QSGItemPrivate::get(item):0;
    if (p) {
        m_next = p->keyHandler;
        p->keyHandler = this;
    }
}

QSGItemKeyFilter::~QSGItemKeyFilter()
{
}

void QSGItemKeyFilter::keyPressed(QKeyEvent *event, bool post)
{
    if (m_next) m_next->keyPressed(event, post);
}

void QSGItemKeyFilter::keyReleased(QKeyEvent *event, bool post)
{
    if (m_next) m_next->keyReleased(event, post);
}

void QSGItemKeyFilter::inputMethodEvent(QInputMethodEvent *event, bool post)
{
    if (m_next)
        m_next->inputMethodEvent(event, post);
    else
        event->ignore();
}

QVariant QSGItemKeyFilter::inputMethodQuery(Qt::InputMethodQuery query) const
{
    if (m_next) return m_next->inputMethodQuery(query);
    return QVariant();
}

void QSGItemKeyFilter::componentComplete()
{
    if (m_next) m_next->componentComplete();
}

QSGKeyNavigationAttached::QSGKeyNavigationAttached(QObject *parent)
: QObject(*(new QSGKeyNavigationAttachedPrivate), parent),
  QSGItemKeyFilter(qobject_cast<QSGItem*>(parent))
{
    m_processPost = true;
}

QSGKeyNavigationAttached *
QSGKeyNavigationAttached::qmlAttachedProperties(QObject *obj)
{
    return new QSGKeyNavigationAttached(obj);
}

QSGItem *QSGKeyNavigationAttached::left() const
{
    Q_D(const QSGKeyNavigationAttached);
    return d->left;
}

void QSGKeyNavigationAttached::setLeft(QSGItem *i)
{
    Q_D(QSGKeyNavigationAttached);
    if (d->left == i)
        return;
    d->left = i;
    d->leftSet = true;
    QSGKeyNavigationAttached* other =
            qobject_cast<QSGKeyNavigationAttached*>(qmlAttachedPropertiesObject<QSGKeyNavigationAttached>(i));
    if (other && !other->d_func()->rightSet){
        other->d_func()->right = qobject_cast<QSGItem*>(parent());
        emit other->rightChanged();
    }
    emit leftChanged();
}

QSGItem *QSGKeyNavigationAttached::right() const
{
    Q_D(const QSGKeyNavigationAttached);
    return d->right;
}

void QSGKeyNavigationAttached::setRight(QSGItem *i)
{
    Q_D(QSGKeyNavigationAttached);
    if (d->right == i)
        return;
    d->right = i;
    d->rightSet = true;
    QSGKeyNavigationAttached* other =
            qobject_cast<QSGKeyNavigationAttached*>(qmlAttachedPropertiesObject<QSGKeyNavigationAttached>(i));
    if (other && !other->d_func()->leftSet){
        other->d_func()->left = qobject_cast<QSGItem*>(parent());
        emit other->leftChanged();
    }
    emit rightChanged();
}

QSGItem *QSGKeyNavigationAttached::up() const
{
    Q_D(const QSGKeyNavigationAttached);
    return d->up;
}

void QSGKeyNavigationAttached::setUp(QSGItem *i)
{
    Q_D(QSGKeyNavigationAttached);
    if (d->up == i)
        return;
    d->up = i;
    d->upSet = true;
    QSGKeyNavigationAttached* other =
            qobject_cast<QSGKeyNavigationAttached*>(qmlAttachedPropertiesObject<QSGKeyNavigationAttached>(i));
    if (other && !other->d_func()->downSet){
        other->d_func()->down = qobject_cast<QSGItem*>(parent());
        emit other->downChanged();
    }
    emit upChanged();
}

QSGItem *QSGKeyNavigationAttached::down() const
{
    Q_D(const QSGKeyNavigationAttached);
    return d->down;
}

void QSGKeyNavigationAttached::setDown(QSGItem *i)
{
    Q_D(QSGKeyNavigationAttached);
    if (d->down == i)
        return;
    d->down = i;
    d->downSet = true;
    QSGKeyNavigationAttached* other =
            qobject_cast<QSGKeyNavigationAttached*>(qmlAttachedPropertiesObject<QSGKeyNavigationAttached>(i));
    if(other && !other->d_func()->upSet){
        other->d_func()->up = qobject_cast<QSGItem*>(parent());
        emit other->upChanged();
    }
    emit downChanged();
}

QSGItem *QSGKeyNavigationAttached::tab() const
{
    Q_D(const QSGKeyNavigationAttached);
    return d->tab;
}

void QSGKeyNavigationAttached::setTab(QSGItem *i)
{
    Q_D(QSGKeyNavigationAttached);
    if (d->tab == i)
        return;
    d->tab = i;
    d->tabSet = true;
    QSGKeyNavigationAttached* other =
            qobject_cast<QSGKeyNavigationAttached*>(qmlAttachedPropertiesObject<QSGKeyNavigationAttached>(i));
    if(other && !other->d_func()->backtabSet){
        other->d_func()->backtab = qobject_cast<QSGItem*>(parent());
        emit other->backtabChanged();
    }
    emit tabChanged();
}

QSGItem *QSGKeyNavigationAttached::backtab() const
{
    Q_D(const QSGKeyNavigationAttached);
    return d->backtab;
}

void QSGKeyNavigationAttached::setBacktab(QSGItem *i)
{
    Q_D(QSGKeyNavigationAttached);
    if (d->backtab == i)
        return;
    d->backtab = i;
    d->backtabSet = true;
    QSGKeyNavigationAttached* other =
            qobject_cast<QSGKeyNavigationAttached*>(qmlAttachedPropertiesObject<QSGKeyNavigationAttached>(i));
    if(other && !other->d_func()->tabSet){
        other->d_func()->tab = qobject_cast<QSGItem*>(parent());
        emit other->tabChanged();
    }
    emit backtabChanged();
}

QSGKeyNavigationAttached::Priority QSGKeyNavigationAttached::priority() const
{
    return m_processPost ? AfterItem : BeforeItem;
}

void QSGKeyNavigationAttached::setPriority(Priority order)
{
    bool processPost = order == AfterItem;
    if (processPost != m_processPost) {
        m_processPost = processPost;
        emit priorityChanged();
    }
}

void QSGKeyNavigationAttached::keyPressed(QKeyEvent *event, bool post)
{
    Q_D(QSGKeyNavigationAttached);
    event->ignore();

    if (post != m_processPost) {
        QSGItemKeyFilter::keyPressed(event, post);
        return;
    }

    bool mirror = false;
    switch(event->key()) {
    case Qt::Key_Left: {
        if (QSGItem *parentItem = qobject_cast<QSGItem*>(parent()))
            mirror = QSGItemPrivate::get(parentItem)->effectiveLayoutMirror;
        QSGItem* leftItem = mirror ? d->right : d->left;
        if (leftItem) {
            setFocusNavigation(leftItem, mirror ? "right" : "left");
            event->accept();
        }
        break;
    }
    case Qt::Key_Right: {
        if (QSGItem *parentItem = qobject_cast<QSGItem*>(parent()))
            mirror = QSGItemPrivate::get(parentItem)->effectiveLayoutMirror;
        QSGItem* rightItem = mirror ? d->left : d->right;
        if (rightItem) {
            setFocusNavigation(rightItem, mirror ? "left" : "right");
            event->accept();
        }
        break;
    }
    case Qt::Key_Up:
        if (d->up) {
            setFocusNavigation(d->up, "up");
            event->accept();
        }
        break;
    case Qt::Key_Down:
        if (d->down) {
            setFocusNavigation(d->down, "down");
            event->accept();
        }
        break;
    case Qt::Key_Tab:
        if (d->tab) {
            setFocusNavigation(d->tab, "tab");
            event->accept();
        }
        break;
    case Qt::Key_Backtab:
        if (d->backtab) {
            setFocusNavigation(d->backtab, "backtab");
            event->accept();
        }
        break;
    default:
        break;
    }

    if (!event->isAccepted()) QSGItemKeyFilter::keyPressed(event, post);
}

void QSGKeyNavigationAttached::keyReleased(QKeyEvent *event, bool post)
{
    Q_D(QSGKeyNavigationAttached);
    event->ignore();

    if (post != m_processPost) {
        QSGItemKeyFilter::keyReleased(event, post);
        return;
    }

    bool mirror = false;
    switch(event->key()) {
    case Qt::Key_Left:
        if (QSGItem *parentItem = qobject_cast<QSGItem*>(parent()))
            mirror = QSGItemPrivate::get(parentItem)->effectiveLayoutMirror;
        if (mirror ? d->right : d->left)
            event->accept();
        break;
    case Qt::Key_Right:
        if (QSGItem *parentItem = qobject_cast<QSGItem*>(parent()))
            mirror = QSGItemPrivate::get(parentItem)->effectiveLayoutMirror;
        if (mirror ? d->left : d->right)
            event->accept();
        break;
    case Qt::Key_Up:
        if (d->up) {
            event->accept();
        }
        break;
    case Qt::Key_Down:
        if (d->down) {
            event->accept();
        }
        break;
    case Qt::Key_Tab:
        if (d->tab) {
            event->accept();
        }
        break;
    case Qt::Key_Backtab:
        if (d->backtab) {
            event->accept();
        }
        break;
    default:
        break;
    }

    if (!event->isAccepted()) QSGItemKeyFilter::keyReleased(event, post);
}

void QSGKeyNavigationAttached::setFocusNavigation(QSGItem *currentItem, const char *dir)
{
    QSGItem *initialItem = currentItem;
    bool isNextItem = false;
    do {
        isNextItem = false;
        if (currentItem->isVisible() && currentItem->isEnabled()) {
            currentItem->setFocus(true);
        } else {
            QObject *attached =
                qmlAttachedPropertiesObject<QSGKeyNavigationAttached>(currentItem, false);
            if (attached) {
                QSGItem *tempItem = qvariant_cast<QSGItem*>(attached->property(dir));
                if (tempItem) {
                    currentItem = tempItem;
                    isNextItem = true;
                }
            }
        }
    }
    while (currentItem != initialItem && isNextItem);
}

const QSGKeysAttached::SigMap QSGKeysAttached::sigMap[] = {
    { Qt::Key_Left, "leftPressed" },
    { Qt::Key_Right, "rightPressed" },
    { Qt::Key_Up, "upPressed" },
    { Qt::Key_Down, "downPressed" },
    { Qt::Key_Tab, "tabPressed" },
    { Qt::Key_Backtab, "backtabPressed" },
    { Qt::Key_Asterisk, "asteriskPressed" },
    { Qt::Key_NumberSign, "numberSignPressed" },
    { Qt::Key_Escape, "escapePressed" },
    { Qt::Key_Return, "returnPressed" },
    { Qt::Key_Enter, "enterPressed" },
    { Qt::Key_Delete, "deletePressed" },
    { Qt::Key_Space, "spacePressed" },
    { Qt::Key_Back, "backPressed" },
    { Qt::Key_Cancel, "cancelPressed" },
    { Qt::Key_Select, "selectPressed" },
    { Qt::Key_Yes, "yesPressed" },
    { Qt::Key_No, "noPressed" },
    { Qt::Key_Context1, "context1Pressed" },
    { Qt::Key_Context2, "context2Pressed" },
    { Qt::Key_Context3, "context3Pressed" },
    { Qt::Key_Context4, "context4Pressed" },
    { Qt::Key_Call, "callPressed" },
    { Qt::Key_Hangup, "hangupPressed" },
    { Qt::Key_Flip, "flipPressed" },
    { Qt::Key_Menu, "menuPressed" },
    { Qt::Key_VolumeUp, "volumeUpPressed" },
    { Qt::Key_VolumeDown, "volumeDownPressed" },
    { 0, 0 }
};

bool QSGKeysAttachedPrivate::isConnected(const char *signalName)
{
    return isSignalConnected(signalIndex(signalName));
}

QSGKeysAttached::QSGKeysAttached(QObject *parent)
: QObject(*(new QSGKeysAttachedPrivate), parent),
  QSGItemKeyFilter(qobject_cast<QSGItem*>(parent))
{
    Q_D(QSGKeysAttached);
    m_processPost = false;
    d->item = qobject_cast<QSGItem*>(parent);
}

QSGKeysAttached::~QSGKeysAttached()
{
}

QSGKeysAttached::Priority QSGKeysAttached::priority() const
{
    return m_processPost ? AfterItem : BeforeItem;
}

void QSGKeysAttached::setPriority(Priority order)
{
    bool processPost = order == AfterItem;
    if (processPost != m_processPost) {
        m_processPost = processPost;
        emit priorityChanged();
    }
}

void QSGKeysAttached::componentComplete()
{
    Q_D(QSGKeysAttached);
    if (d->item) {
        for (int ii = 0; ii < d->targets.count(); ++ii) {
            QSGItem *targetItem = d->targets.at(ii);
            if (targetItem && (targetItem->flags() & QSGItem::ItemAcceptsInputMethod)) {
                d->item->setFlag(QSGItem::ItemAcceptsInputMethod);
                break;
            }
        }
    }
}

void QSGKeysAttached::keyPressed(QKeyEvent *event, bool post)
{
    Q_D(QSGKeysAttached);
    if (post != m_processPost || !d->enabled || d->inPress) {
        event->ignore();
        QSGItemKeyFilter::keyPressed(event, post);
        return;
    }

    // first process forwards
    if (d->item && d->item->canvas()) {
        d->inPress = true;
        for (int ii = 0; ii < d->targets.count(); ++ii) {
            QSGItem *i = d->targets.at(ii);
            if (i && i->isVisible()) {
                d->item->canvas()->sendEvent(i, event);
                if (event->isAccepted()) {
                    d->inPress = false;
                    return;
                }
            }
        }
        d->inPress = false;
    }

    QSGKeyEvent ke(*event);
    QByteArray keySignal = keyToSignal(event->key());
    if (!keySignal.isEmpty()) {
        keySignal += "(QSGKeyEvent*)";
        if (d->isConnected(keySignal)) {
            // If we specifically handle a key then default to accepted
            ke.setAccepted(true);
            int idx = QSGKeysAttached::staticMetaObject.indexOfSignal(keySignal);
            metaObject()->method(idx).invoke(this, Qt::DirectConnection, Q_ARG(QSGKeyEvent*, &ke));
        }
    }
    if (!ke.isAccepted())
        emit pressed(&ke);
    event->setAccepted(ke.isAccepted());

    if (!event->isAccepted()) QSGItemKeyFilter::keyPressed(event, post);
}

void QSGKeysAttached::keyReleased(QKeyEvent *event, bool post)
{
    Q_D(QSGKeysAttached);
    if (post != m_processPost || !d->enabled || d->inRelease) {
        event->ignore();
        QSGItemKeyFilter::keyReleased(event, post);
        return;
    }

    if (d->item && d->item->canvas()) {
        d->inRelease = true;
        for (int ii = 0; ii < d->targets.count(); ++ii) {
            QSGItem *i = d->targets.at(ii);
            if (i && i->isVisible()) {
                d->item->canvas()->sendEvent(i, event);
                if (event->isAccepted()) {
                    d->inRelease = false;
                    return;
                }
            }
        }
        d->inRelease = false;
    }

    QSGKeyEvent ke(*event);
    emit released(&ke);
    event->setAccepted(ke.isAccepted());

    if (!event->isAccepted()) QSGItemKeyFilter::keyReleased(event, post);
}

void QSGKeysAttached::inputMethodEvent(QInputMethodEvent *event, bool post)
{
    Q_D(QSGKeysAttached);
    if (post == m_processPost && d->item && !d->inIM && d->item->canvas()) {
        d->inIM = true;
        for (int ii = 0; ii < d->targets.count(); ++ii) {
            QSGItem *i = d->targets.at(ii);
            if (i && i->isVisible() && (i->flags() & QSGItem::ItemAcceptsInputMethod)) {
                d->item->canvas()->sendEvent(i, event);
                if (event->isAccepted()) {
                    d->imeItem = i;
                    d->inIM = false;
                    return;
                }
            }
        }
        d->inIM = false;
    }
    QSGItemKeyFilter::inputMethodEvent(event, post);
}

QVariant QSGKeysAttached::inputMethodQuery(Qt::InputMethodQuery query) const
{
    Q_D(const QSGKeysAttached);
    if (d->item) {
        for (int ii = 0; ii < d->targets.count(); ++ii) {
            QSGItem *i = d->targets.at(ii);
            if (i && i->isVisible() && (i->flags() & QSGItem::ItemAcceptsInputMethod) && i == d->imeItem) { 
                //### how robust is i == d->imeItem check?
                QVariant v = i->inputMethodQuery(query);
                if (v.userType() == QVariant::RectF)
                    v = d->item->mapRectFromItem(i, v.toRectF());  //### cost?
                return v;
            }
        }
    }
    return QSGItemKeyFilter::inputMethodQuery(query);
}

QSGKeysAttached *QSGKeysAttached::qmlAttachedProperties(QObject *obj)
{
    return new QSGKeysAttached(obj);
}


QSGLayoutMirroringAttached::QSGLayoutMirroringAttached(QObject *parent) : QObject(parent), itemPrivate(0)
{
    if (QSGItem *item = qobject_cast<QSGItem*>(parent)) {
        itemPrivate = QSGItemPrivate::get(item);
        itemPrivate->attachedLayoutDirection = this;
    } else
        qmlInfo(parent) << tr("LayoutDirection attached property only works with Items");
}

QSGLayoutMirroringAttached * QSGLayoutMirroringAttached::qmlAttachedProperties(QObject *object)
{
    return new QSGLayoutMirroringAttached(object);
}

bool QSGLayoutMirroringAttached::enabled() const
{
    return itemPrivate ? itemPrivate->effectiveLayoutMirror : false;
}

void QSGLayoutMirroringAttached::setEnabled(bool enabled)
{
    if (!itemPrivate)
        return;

    itemPrivate->isMirrorImplicit = false;
    if (enabled != itemPrivate->effectiveLayoutMirror) {
        itemPrivate->setLayoutMirror(enabled);
        if (itemPrivate->inheritMirrorFromItem)
             itemPrivate->resolveLayoutMirror();
    }
}

void QSGLayoutMirroringAttached::resetEnabled()
{
    if (itemPrivate && !itemPrivate->isMirrorImplicit) {
        itemPrivate->isMirrorImplicit = true;
        itemPrivate->resolveLayoutMirror();
    }
}

bool QSGLayoutMirroringAttached::childrenInherit() const
{
    return itemPrivate ? itemPrivate->inheritMirrorFromItem : false;
}

void QSGLayoutMirroringAttached::setChildrenInherit(bool childrenInherit) {
    if (itemPrivate && childrenInherit != itemPrivate->inheritMirrorFromItem) {
        itemPrivate->inheritMirrorFromItem = childrenInherit;
        itemPrivate->resolveLayoutMirror();
        childrenInheritChanged();
    }
}

void QSGItemPrivate::resolveLayoutMirror()
{
    Q_Q(QSGItem);
    if (QSGItem *parentItem = q->parentItem()) {
        QSGItemPrivate *parentPrivate = QSGItemPrivate::get(parentItem);
        setImplicitLayoutMirror(parentPrivate->inheritedLayoutMirror, parentPrivate->inheritMirrorFromParent);
    } else {
        setImplicitLayoutMirror(isMirrorImplicit ? false : effectiveLayoutMirror, inheritMirrorFromItem);
    }
}

void QSGItemPrivate::setImplicitLayoutMirror(bool mirror, bool inherit)
{
    inherit = inherit || inheritMirrorFromItem;
    if (!isMirrorImplicit && inheritMirrorFromItem)
        mirror = effectiveLayoutMirror;
    if (mirror == inheritedLayoutMirror && inherit == inheritMirrorFromParent)
        return;

    inheritMirrorFromParent = inherit;
    inheritedLayoutMirror = inheritMirrorFromParent ? mirror : false;

    if (isMirrorImplicit)
        setLayoutMirror(inherit ? inheritedLayoutMirror : false);
    for (int i = 0; i < childItems.count(); ++i) {
        if (QSGItem *child = qobject_cast<QSGItem *>(childItems.at(i))) {
            QSGItemPrivate *childPrivate = QSGItemPrivate::get(child);
            childPrivate->setImplicitLayoutMirror(inheritedLayoutMirror, inheritMirrorFromParent);
        }
    }
}

void QSGItemPrivate::setLayoutMirror(bool mirror)
{
    if (mirror != effectiveLayoutMirror) {
        effectiveLayoutMirror = mirror;
        if (_anchors) {
            QSGAnchorsPrivate *anchor_d = QSGAnchorsPrivate::get(_anchors);
            anchor_d->fillChanged();
            anchor_d->centerInChanged();
            anchor_d->updateHorizontalAnchors();
            emit _anchors->mirroredChanged();
        }
        mirrorChange();
        if (attachedLayoutDirection) {
            emit attachedLayoutDirection->enabledChanged();
        }
    }
}

QSGItem::QSGItem(QSGItem* parent)
: QObject(*(new QSGItemPrivate), parent)
{
    Q_D(QSGItem);
    d->init(parent);
}

QSGItem::QSGItem(QSGItemPrivate &dd, QSGItem *parent)
: QObject(dd, parent)
{
    Q_D(QSGItem);
    d->init(parent);
}

QSGItem::~QSGItem()
{
    Q_D(QSGItem);

    // XXX todo - optimize
    setParentItem(0);
    while (!d->childItems.isEmpty())
        d->childItems.first()->setParentItem(0);
    
    for (int ii = 0; ii < d->changeListeners.count(); ++ii) {
        QSGAnchorsPrivate *anchor = d->changeListeners.at(ii).listener->anchorPrivate();
        if (anchor)
            anchor->clearItem(this);
    }

    // XXX todo - the original checks if the parent is being destroyed
    for (int ii = 0; ii < d->changeListeners.count(); ++ii) {
        QSGAnchorsPrivate *anchor = d->changeListeners.at(ii).listener->anchorPrivate();
        if (anchor && anchor->item && anchor->item->parent() != this) //child will be deleted anyway
            anchor->updateOnComplete();
    }
    
    for (int ii = 0; ii < d->changeListeners.count(); ++ii) {
        const QSGItemPrivate::ChangeListener &change = d->changeListeners.at(ii);
        if (change.types & QSGItemPrivate::Destroyed)
            change.listener->itemDestroyed(this);
    }
    d->changeListeners.clear();
    delete d->_anchorLines; d->_anchorLines = 0;
    delete d->_anchors; d->_anchors = 0;
    delete d->_stateGroup; d->_stateGroup = 0;
    delete d->_contents; d->_contents = 0;
}

void QSGItem::setParentItem(QSGItem *parentItem)
{
    Q_D(QSGItem);
    if (parentItem == d->parentItem)
        return;

    d->removeFromDirtyList();

    QSGItem *oldParentItem = d->parentItem;
    QSGItem *scopeFocusedItem = 0;

    if (oldParentItem) {
        QSGItemPrivate *op = QSGItemPrivate::get(oldParentItem);

        QSGItem *scopeItem = 0;

        if (d->canvas && hasFocus()) {
            scopeItem = oldParentItem;
            while (!scopeItem->isFocusScope()) scopeItem = scopeItem->parentItem();
            scopeFocusedItem = this;
        } else if (d->canvas && !isFocusScope() && d->subFocusItem) {
            scopeItem = oldParentItem;
            while (!scopeItem->isFocusScope()) scopeItem = scopeItem->parentItem();
            scopeFocusedItem = d->subFocusItem;
        } 

        if (scopeFocusedItem) 
            QSGCanvasPrivate::get(d->canvas)->clearFocusInScope(scopeItem, scopeFocusedItem, 
                                                                QSGCanvasPrivate::DontChangeFocusProperty);

        op->removeChild(this);
    }

    d->parentItem = parentItem;

    QSGCanvas *parentCanvas = parentItem?QSGItemPrivate::get(parentItem)->canvas:0;
    if (d->canvas != parentCanvas) {
        if (d->canvas && d->itemNodeInstance)
            QSGCanvasPrivate::get(d->canvas)->cleanup(d->itemNodeInstance);

        QSGItemPrivate::InitializationState initState;
        initState.clear();
        d->initCanvas(&initState, parentCanvas);
    }

    d->dirty(QSGItemPrivate::ParentChanged);
    
    if (d->parentItem)
        QSGItemPrivate::get(d->parentItem)->addChild(this);

    d->setEffectiveVisibleRecur(d->calcEffectiveVisible());
    d->setEffectiveEnableRecur(d->calcEffectiveEnable());

    if (scopeFocusedItem && d->parentItem && d->canvas) {
        // We need to test whether this item becomes scope focused
        QSGItem *scopeItem = 0;
        scopeItem = d->parentItem;
        while (!scopeItem->isFocusScope()) scopeItem = scopeItem->parentItem();

        if (scopeItem->scopedFocusItem()) {
            QSGItemPrivate::get(scopeFocusedItem)->focus = false;
            emit scopeFocusedItem->focusChanged(false);
        } else {
            QSGCanvasPrivate::get(d->canvas)->setFocusInScope(scopeItem, scopeFocusedItem, 
                                                              QSGCanvasPrivate::DontChangeFocusProperty);
        }
    }

    d->resolveLayoutMirror();

    d->itemChange(ItemParentHasChanged, d->parentItem);

    emit parentChanged(d->parentItem);
}

void QSGItem::stackBefore(const QSGItem *sibling)
{
    Q_D(QSGItem);
    if (!sibling || sibling == this || !d->parentItem || d->parentItem != QSGItemPrivate::get(sibling)->parentItem) {
        qWarning("QSGItem::stackBefore: Cannot stack before %p, which must be a sibling", sibling);
        return;
    }

    QSGItemPrivate *parentPrivate = QSGItemPrivate::get(d->parentItem);

    int myIndex = parentPrivate->childItems.indexOf(this);
    int siblingIndex = parentPrivate->childItems.indexOf(const_cast<QSGItem *>(sibling));
    
    Q_ASSERT(myIndex != -1 && siblingIndex != -1);

    if (myIndex == siblingIndex - 1)
        return;

    parentPrivate->childItems.removeAt(myIndex);

    if (myIndex < siblingIndex) --siblingIndex;

    parentPrivate->childItems.insert(siblingIndex, this);

    parentPrivate->dirty(QSGItemPrivate::ChildrenStackingChanged);

    for (int ii = qMin(siblingIndex, myIndex); ii < parentPrivate->childItems.count(); ++ii) 
        QSGItemPrivate::get(parentPrivate->childItems.at(ii))->siblingOrderChanged();
}

void QSGItem::stackAfter(const QSGItem *sibling)
{
    Q_D(QSGItem);
    if (!sibling || sibling == this || !d->parentItem || d->parentItem != QSGItemPrivate::get(sibling)->parentItem) {
        qWarning("QSGItem::stackAfter: Cannot stack after %p, which must be a sibling", sibling);
        return;
    }

    QSGItemPrivate *parentPrivate = QSGItemPrivate::get(d->parentItem);

    int myIndex = parentPrivate->childItems.indexOf(this);
    int siblingIndex = parentPrivate->childItems.indexOf(const_cast<QSGItem *>(sibling));
    
    Q_ASSERT(myIndex != -1 && siblingIndex != -1);

    if (myIndex == siblingIndex + 1)
        return;

    parentPrivate->childItems.removeAt(myIndex);

    if (myIndex < siblingIndex) --siblingIndex;

    parentPrivate->childItems.insert(siblingIndex + 1, this);

    parentPrivate->dirty(QSGItemPrivate::ChildrenStackingChanged);

    for (int ii = qMin(myIndex, siblingIndex + 1); ii < parentPrivate->childItems.count(); ++ii) 
        QSGItemPrivate::get(parentPrivate->childItems.at(ii))->siblingOrderChanged();
}

/*!
    Returns the QSGItem parent of this item.
*/
QSGItem *QSGItem::parentItem() const
{
    Q_D(const QSGItem);
    return d->parentItem;
}

QSGEngine *QSGItem::sceneGraphEngine() const
{
    return canvas()->sceneGraphEngine();
}

QSGCanvas *QSGItem::canvas() const 
{ 
    Q_D(const QSGItem);
    return d->canvas;
}

static bool itemZOrder_sort(QSGItem *lhs, QSGItem *rhs)
{
    return lhs->z() < rhs->z();
}

QList<QSGItem *> QSGItemPrivate::paintOrderChildItems() const
{
    // XXX todo - optimize, don't sort and return items that are
    // ignored anyway, like invisible or disabled items.
    QList<QSGItem *> items = childItems;
    qStableSort(items.begin(), items.end(), itemZOrder_sort);
    return items;
}

void QSGItemPrivate::addChild(QSGItem *child)
{
    Q_Q(QSGItem);

    Q_ASSERT(!childItems.contains(child));

    childItems.append(child);

    dirty(QSGItemPrivate::ChildrenChanged);

    itemChange(QSGItem::ItemChildAddedChange, child);

    emit q->childrenChanged();
}

void QSGItemPrivate::removeChild(QSGItem *child)
{
    Q_Q(QSGItem);

    Q_ASSERT(child);
    Q_ASSERT(childItems.contains(child));
    childItems.removeOne(child);
    Q_ASSERT(!childItems.contains(child));

    dirty(QSGItemPrivate::ChildrenChanged);

    itemChange(QSGItem::ItemChildRemovedChange, child);

    emit q->childrenChanged();
}

void QSGItemPrivate::InitializationState::clear() 
{ 
    focusScope = 0; 
}

void QSGItemPrivate::InitializationState::clear(QSGItem *fs) 
{ 
    focusScope = fs;
}

QSGItem *QSGItemPrivate::InitializationState::getFocusScope(QSGItem *item)
{
    if (!focusScope) {
        QSGItem *fs = item->parentItem();
        while (!fs->isFocusScope())
            fs = fs->parentItem();
        focusScope = fs;
    }
    return focusScope;
}

void QSGItemPrivate::initCanvas(InitializationState *state, QSGCanvas *c)
{
    Q_Q(QSGItem);

    if (canvas) {
        removeFromDirtyList();
        QSGCanvasPrivate *c = QSGCanvasPrivate::get(canvas);
        if (polishScheduled)
            c->itemsToPolish.remove(q);
        if (c->mouseGrabberItem == q)
            c->mouseGrabberItem = 0;
    }

    canvas = c;

    if (canvas && polishScheduled) 
        QSGCanvasPrivate::get(canvas)->itemsToPolish.insert(q);

    if (canvas && hoverEnabled && !canvas->hasMouseTracking())
        canvas->setMouseTracking(true);

    // XXX todo - why aren't these added to the destroy list?
    itemNodeInstance = 0;
    opacityNode = 0;
    clipNode = 0;
    rootNode = 0;
    groupNode = 0;
    paintNode = 0;
    paintNodeIndex = 0;

    InitializationState _dummy;
    InitializationState *childState = state;

    if (c && q->isFocusScope()) {
        _dummy.clear(q);
        childState = &_dummy;
    }

    for (int ii = 0; ii < childItems.count(); ++ii) {
        QSGItem *child = childItems.at(ii);
        QSGItemPrivate::get(child)->initCanvas(childState, c);
    }

    if (c && focus) {
        // Fixup
        if (state->getFocusScope(q)->scopedFocusItem()) {
            focus = false;
            emit q->focusChanged(false);
        } else {
            QSGCanvasPrivate::get(canvas)->setFocusInScope(state->getFocusScope(q), q);
        }
    }

    dirty(Canvas);

    itemChange(QSGItem::ItemSceneChange, c);
}

/*!
Returns a transform that maps points from canvas space into item space.
*/
QTransform QSGItemPrivate::canvasToItemTransform() const
{
    // XXX todo - optimize
    return itemToCanvasTransform().inverted();
}

/*!
Returns a transform that maps points from item space into canvas space.
*/
QTransform QSGItemPrivate::itemToCanvasTransform() const
{
    // XXX todo
    QTransform rv = parentItem?QSGItemPrivate::get(parentItem)->itemToCanvasTransform():QTransform();
    itemToParentTransform(rv);
    return rv;
}

/*!
Motifies \a t with this items local transform relative to its parent.
*/
void QSGItemPrivate::itemToParentTransform(QTransform &t) const
{
    if (x || y)
        t.translate(x, y);

    if (!transforms.isEmpty()) {
        QMatrix4x4 m(t);
        for (int ii = transforms.count() - 1; ii >= 0; --ii)
            transforms.at(ii)->applyTo(&m);
        t = m.toTransform();
    }

    if (scale != 1. || rotation != 0.) {
        QPointF tp = computeTransformOrigin();
        t.translate(tp.x(), tp.y());
        t.scale(scale, scale);
        t.rotate(rotation);
        t.translate(-tp.x(), -tp.y());
    }
}

bool QSGItem::isComponentComplete() const
{
    Q_D(const QSGItem);
    return d->componentComplete;
}

QSGItemPrivate::QSGItemPrivate()
: _anchors(0), _contents(0), baselineOffset(0), _anchorLines(0), _stateGroup(0), origin(QSGItem::Center), 
    
  flags(0), widthValid(false), heightValid(false), componentComplete(true), 
  keepMouse(false), hoverEnabled(false), smooth(false), focus(false), activeFocus(false), notifiedFocus(false),
  notifiedActiveFocus(false), filtersChildMouseEvents(false), explicitVisible(true), 
  effectiveVisible(true), explicitEnable(true), effectiveEnable(true), polishScheduled(false),
  inheritedLayoutMirror(false), effectiveLayoutMirror(false), isMirrorImplicit(true),
  inheritMirrorFromParent(false), inheritMirrorFromItem(false),

  canvas(0), parentItem(0),

  subFocusItem(0),

  x(0), y(0), width(0), height(0), implicitWidth(0), implicitHeight(0), 
  z(0), scale(1), rotation(0), opacity(1),

  attachedLayoutDirection(0), acceptedMouseButtons(0),
  imHints(Qt::ImhNone),
  
  keyHandler(0),

  dirtyAttributes(0), nextDirtyItem(0), prevDirtyItem(0),

  itemNodeInstance(0), opacityNode(0), clipNode(0), rootNode(0), groupNode(0), paintNode(0)
  , paintNodeIndex(0), effectRefCount(0), hideRefCount(0)
{
}

void QSGItemPrivate::init(QSGItem *parent)
{
    Q_Q(QSGItem);
    baselineOffset.invalidate();

    if (parent) {
        q->setParentItem(parent);
        QSGItemPrivate *parentPrivate = QSGItemPrivate::get(parent);
        setImplicitLayoutMirror(parentPrivate->inheritedLayoutMirror, parentPrivate->inheritMirrorFromParent);
    }
}

void QSGItemPrivate::data_append(QDeclarativeListProperty<QObject> *prop, QObject *o)
{
    if (!o)
        return;

    QSGItem *that = static_cast<QSGItem *>(prop->object);

    // This test is measurably (albeit only slightly) faster than qobject_cast<>()
    const QMetaObject *mo = o->metaObject();
    while (mo && mo != &QSGItem::staticMetaObject) {
        if (mo == &QDeclarativeItem::staticMetaObject) 
            qWarning("Cannot add a QtQuick 1.0 item (%s) into a QtQuick 2.0 scene!", o->metaObject()->className());
        mo = mo->d.superdata;
    }

    if (mo) {
        QSGItem *item = static_cast<QSGItem *>(o);
        item->setParentItem(that);
    } else {
        // XXX todo - do we really want this behavior?
        o->setParent(that);
    }
}

int QSGItemPrivate::data_count(QDeclarativeListProperty<QObject> *prop)
{
    Q_UNUSED(prop);
    // XXX todo
    return 0;
}

QObject *QSGItemPrivate::data_at(QDeclarativeListProperty<QObject> *prop, int i)
{
    Q_UNUSED(prop);
    Q_UNUSED(i);
    // XXX todo
    return 0;
}

void QSGItemPrivate::data_clear(QDeclarativeListProperty<QObject> *prop)
{
    Q_UNUSED(prop);
    // XXX todo
}

QObject *QSGItemPrivate::resources_at(QDeclarativeListProperty<QObject> *prop, int index)
{
    const QObjectList children = prop->object->children();
    if (index < children.count())
        return children.at(index);
    else
        return 0;
}

void QSGItemPrivate::resources_append(QDeclarativeListProperty<QObject> *prop, QObject *o)
{
    // XXX todo - do we really want this behavior?
    o->setParent(prop->object);
}

int QSGItemPrivate::resources_count(QDeclarativeListProperty<QObject> *prop)
{
    return prop->object->children().count();
}

void QSGItemPrivate::resources_clear(QDeclarativeListProperty<QObject> *prop)
{
    // XXX todo - do we really want this behavior?
    const QObjectList children = prop->object->children();
    for (int index = 0; index < children.count(); index++)
        children.at(index)->setParent(0);
}

QSGItem *QSGItemPrivate::children_at(QDeclarativeListProperty<QSGItem> *prop, int index)
{
    QSGItemPrivate *p = QSGItemPrivate::get(static_cast<QSGItem *>(prop->object));
    if (index >= p->childItems.count() || index < 0)
        return 0;
    else
        return p->childItems.at(index);
}

void QSGItemPrivate::children_append(QDeclarativeListProperty<QSGItem> *prop, QSGItem *o)
{
    if (!o)
        return;

    QSGItem *that = static_cast<QSGItem *>(prop->object);
    if (o->parentItem() == that)
        o->setParentItem(0);

    o->setParentItem(that);
}

int QSGItemPrivate::children_count(QDeclarativeListProperty<QSGItem> *prop)
{
    QSGItemPrivate *p = QSGItemPrivate::get(static_cast<QSGItem *>(prop->object));
    return p->childItems.count();
}

void QSGItemPrivate::children_clear(QDeclarativeListProperty<QSGItem> *prop)
{
    QSGItem *that = static_cast<QSGItem *>(prop->object);
    QSGItemPrivate *p = QSGItemPrivate::get(that);
    while (!p->childItems.isEmpty())
        p->childItems.at(0)->setParentItem(0);
}

int QSGItemPrivate::transform_count(QDeclarativeListProperty<QSGTransform> *prop)
{
    QSGItem *that = static_cast<QSGItem *>(prop->object);
    return QSGItemPrivate::get(that)->transforms.count();
}

void QSGTransform::appendToItem(QSGItem *item)
{
    Q_D(QSGTransform);
    if (!item)
        return;

    QSGItemPrivate *p = QSGItemPrivate::get(item);

    if (!d->items.isEmpty() && !p->transforms.isEmpty() && p->transforms.contains(this)) {
        p->transforms.removeOne(this);
        p->transforms.append(this);
    } else {
        p->transforms.append(this);
        d->items.append(item);
    }

    p->dirty(QSGItemPrivate::Transform);
}

void QSGTransform::prependToItem(QSGItem *item)
{
    Q_D(QSGTransform);
    if (!item)
        return;

    QSGItemPrivate *p = QSGItemPrivate::get(item);

    if (!d->items.isEmpty() && !p->transforms.isEmpty() && p->transforms.contains(this)) {
        p->transforms.removeOne(this);
        p->transforms.prepend(this);
    } else {
        p->transforms.prepend(this);
        d->items.append(item);
    }

    p->dirty(QSGItemPrivate::Transform);
}

void QSGItemPrivate::transform_append(QDeclarativeListProperty<QSGTransform> *prop, QSGTransform *transform)
{
    if (!transform)
        return;

    QSGItem *that = static_cast<QSGItem *>(prop->object);
    transform->appendToItem(that);
}

QSGTransform *QSGItemPrivate::transform_at(QDeclarativeListProperty<QSGTransform> *prop, int idx)
{
    QSGItem *that = static_cast<QSGItem *>(prop->object);
    QSGItemPrivate *p = QSGItemPrivate::get(that);

    if (idx < 0 || idx >= p->transforms.count())
        return 0;
    else
        return p->transforms.at(idx);
}

void QSGItemPrivate::transform_clear(QDeclarativeListProperty<QSGTransform> *prop)
{
    QSGItem *that = static_cast<QSGItem *>(prop->object);
    QSGItemPrivate *p = QSGItemPrivate::get(that);

    for (int ii = 0; ii < p->transforms.count(); ++ii) {
        QSGTransform *t = p->transforms.at(ii);
        QSGTransformPrivate *tp = QSGTransformPrivate::get(t);
        tp->items.removeOne(that);
    }

    p->transforms.clear();

    p->dirty(QSGItemPrivate::Transform);
}

QSGAnchors *QSGItemPrivate::anchors() const
{
    if (!_anchors) {
        Q_Q(const QSGItem);
        _anchors = new QSGAnchors(const_cast<QSGItem *>(q));
        if (!componentComplete)
            _anchors->classBegin();
    }
    return _anchors;
}

QSGItemPrivate::AnchorLines *QSGItemPrivate::anchorLines() const 
{
    Q_Q(const QSGItem);
    if (!_anchorLines) _anchorLines =
        new AnchorLines(const_cast<QSGItem *>(q));
    return _anchorLines;
}

void QSGItemPrivate::siblingOrderChanged()
{
    Q_Q(QSGItem);
    for(int ii = 0; ii < changeListeners.count(); ++ii) {
        const QSGItemPrivate::ChangeListener &change = changeListeners.at(ii);
        if (change.types & QSGItemPrivate::SiblingOrder) {
            change.listener->itemSiblingOrderChanged(q);
        }
    }
}

QDeclarativeListProperty<QObject> QSGItemPrivate::data()
{
    return QDeclarativeListProperty<QObject>(q_func(), 0, QSGItemPrivate::data_append,
                                             QSGItemPrivate::data_count,
                                             QSGItemPrivate::data_at,
                                             QSGItemPrivate::data_clear);
}

QRectF QSGItem::childrenRect()
{
    Q_D(QSGItem);
    if (!d->_contents) {
        d->_contents = new QSGContents(this);
        if (d->componentComplete)
            d->_contents->complete();
    }
    return d->_contents->rectF();
}

QList<QSGItem *> QSGItem::childItems() const
{
    Q_D(const QSGItem);
    return d->childItems;
}

bool QSGItem::clip() const
{
    return flags() & ItemClipsChildrenToShape;
}

void QSGItem::setClip(bool c)
{
    if (clip() == c)
        return;

    setFlag(ItemClipsChildrenToShape, c);

    emit clipChanged(c);
}

void QSGItem::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    Q_D(QSGItem);

    if (d->_anchors)
        QSGAnchorsPrivate::get(d->_anchors)->updateMe();

    for(int ii = 0; ii < d->changeListeners.count(); ++ii) {
        const QSGItemPrivate::ChangeListener &change = d->changeListeners.at(ii);
        if (change.types & QSGItemPrivate::Geometry)
            change.listener->itemGeometryChanged(this, newGeometry, oldGeometry);
    }

    if (newGeometry.x() != oldGeometry.x())
        emit xChanged();
    if (newGeometry.y() != oldGeometry.y())
        emit yChanged();
    if (newGeometry.width() != oldGeometry.width())
        emit widthChanged();
    if (newGeometry.height() != oldGeometry.height())
        emit heightChanged();
}

/*!
    Called by the rendering thread when it is time to sync the state of the QML objects with the
    scene graph objects. The function should return the root of the scene graph subtree for
    this item. \a oldNode is the node that was returned the last time the function was called.

    The main thread is blocked while this function is executed so it is safe to read
    values from the QSGItem instance and other objects in the main thread.

    \warning This is the only function in which it is allowed to make use of scene graph
    objects from the main thread. Use of scene graph objects outside this function will
    result in race conditions and potential crashes.
 */

QSGNode *QSGItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    delete oldNode;
    return 0;
}

QSGTransformNode *QSGItemPrivate::createTransformNode()
{
    return new QSGTransformNode;
}

void QSGItem::updatePolish()
{
}

void QSGItemPrivate::removeItemChangeListener(QSGItemChangeListener *listener, ChangeTypes types)
{
    ChangeListener change(listener, types);
    changeListeners.removeOne(change);
}

void QSGItem::keyPressEvent(QKeyEvent *event)
{
    event->ignore();
}

void QSGItem::keyReleaseEvent(QKeyEvent *event)
{
    event->ignore();
}

void QSGItem::inputMethodEvent(QInputMethodEvent *event)
{
    event->ignore();
}

void QSGItem::focusInEvent(QFocusEvent *)
{
}

void QSGItem::focusOutEvent(QFocusEvent *)
{
}

void QSGItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->ignore();
}

void QSGItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    event->ignore();
}

void QSGItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    event->ignore();
}

void QSGItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    mousePressEvent(event);
}

void QSGItem::mouseUngrabEvent()
{
    // XXX todo
}

void QSGItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    event->ignore();
}

void QSGItem::touchEvent(QTouchEvent *event)
{
    event->ignore();
}

void QSGItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
}

void QSGItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
}

void QSGItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
}

bool QSGItem::childMouseEventFilter(QSGItem *, QEvent *)
{
    return false;
}

Qt::InputMethodHints QSGItem::inputMethodHints() const
{
    Q_D(const QSGItem);
    return d->imHints;
}

void QSGItem::setInputMethodHints(Qt::InputMethodHints hints)
{
    Q_D(QSGItem);
    d->imHints = hints;

    if (!d->canvas || d->canvas->activeFocusItem() != this)
        return;

    QSGCanvasPrivate::get(d->canvas)->updateInputMethodData();
#ifndef QT_NO_IM
    if (d->canvas->hasFocus())
        if (QInputContext *inputContext = d->canvas->inputContext())
            inputContext->update();
#endif
}

void QSGItem::updateMicroFocus()
{
#ifndef QT_NO_IM
    Q_D(QSGItem);
    if (d->canvas && d->canvas->hasFocus())
        if (QInputContext *inputContext = d->canvas->inputContext())
            inputContext->update();
#endif
}

QVariant QSGItem::inputMethodQuery(Qt::InputMethodQuery query) const
{
    Q_D(const QSGItem);
    QVariant v;

    if (d->keyHandler)
        v = d->keyHandler->inputMethodQuery(query);

    return v;
}

QSGAnchorLine QSGItemPrivate::left() const
{
    return anchorLines()->left;
}

QSGAnchorLine QSGItemPrivate::right() const
{
    return anchorLines()->right;
}

QSGAnchorLine QSGItemPrivate::horizontalCenter() const
{
    return anchorLines()->hCenter;
}

QSGAnchorLine QSGItemPrivate::top() const
{
    return anchorLines()->top;
}

QSGAnchorLine QSGItemPrivate::bottom() const
{
    return anchorLines()->bottom;
}

QSGAnchorLine QSGItemPrivate::verticalCenter() const
{
    return anchorLines()->vCenter;
}

QSGAnchorLine QSGItemPrivate::baseline() const
{
    return anchorLines()->baseline;
}

qreal QSGItem::baselineOffset() const
{
    Q_D(const QSGItem);
    if (!d->baselineOffset.isValid()) {
        return 0.0;
    } else
        return d->baselineOffset;
}

void QSGItem::setBaselineOffset(qreal offset)
{
    Q_D(QSGItem);
    if (offset == d->baselineOffset)
        return;

    d->baselineOffset = offset;

    for(int ii = 0; ii < d->changeListeners.count(); ++ii) {
        const QSGItemPrivate::ChangeListener &change = d->changeListeners.at(ii);
        if (change.types & QSGItemPrivate::Geometry) {
            QSGAnchorsPrivate *anchor = change.listener->anchorPrivate();
            if (anchor)
                anchor->updateVerticalAnchors();
        }
    }
    emit baselineOffsetChanged(offset);
}

void QSGItem::update()
{
    Q_D(QSGItem);
    Q_ASSERT(flags() & ItemHasContents);
    d->dirty(QSGItemPrivate::Content);
}

void QSGItem::polish()
{
    Q_D(QSGItem);
    if (!d->polishScheduled) {
        d->polishScheduled = true;
        if (d->canvas)
            QSGCanvasPrivate::get(d->canvas)->itemsToPolish.insert(this);
    }
}

QScriptValue QSGItem::mapFromItem(const QScriptValue &item, qreal x, qreal y) const
{
    QScriptValue sv = QDeclarativeEnginePrivate::getScriptEngine(qmlEngine(this))->newObject();
    QSGItem *itemObj = qobject_cast<QSGItem*>(item.toQObject());
    if (!itemObj && !item.isNull()) {
        qmlInfo(this) << "mapFromItem() given argument \"" << item.toString() << "\" which is neither null nor an Item";
        return 0;
    }

    // If QSGItem::mapFromItem() is called with 0, behaves the same as mapFromScene()
    QPointF p = mapFromItem(itemObj, QPointF(x, y));
    sv.setProperty(QLatin1String("x"), p.x());
    sv.setProperty(QLatin1String("y"), p.y());
    return sv;
}

QTransform QSGItem::itemTransform(QSGItem *other, bool *ok) const
{
    Q_D(const QSGItem);

    // XXX todo - we need to be able to handle common parents better and detect 
    // invalid cases
    if (ok) *ok = true;

    QTransform t = d->itemToCanvasTransform();
    if (other) t *= QSGItemPrivate::get(other)->canvasToItemTransform();

    return t;
}

QScriptValue QSGItem::mapToItem(const QScriptValue &item, qreal x, qreal y) const
{
    QScriptValue sv = QDeclarativeEnginePrivate::getScriptEngine(qmlEngine(this))->newObject();
    QSGItem *itemObj = qobject_cast<QSGItem*>(item.toQObject());
    if (!itemObj && !item.isNull()) {
        qmlInfo(this) << "mapToItem() given argument \"" << item.toString() << "\" which is neither null nor an Item";
        return 0;
    }

    // If QSGItem::mapToItem() is called with 0, behaves the same as mapToScene()
    QPointF p = mapToItem(itemObj, QPointF(x, y));
    sv.setProperty(QLatin1String("x"), p.x());
    sv.setProperty(QLatin1String("y"), p.y());
    return sv;
}

void QSGItem::forceActiveFocus()
{
    setFocus(true);
    QSGItem *parent = parentItem();
    while (parent) {
        if (parent->flags() & QSGItem::ItemIsFocusScope) {
            parent->setFocus(true);
        } 
        parent = parent->parentItem();
    }
}

QSGItem *QSGItem::childAt(qreal x, qreal y) const
{
    // XXX todo - should this include transform etc.?
    const QList<QSGItem *> children = childItems();
    for (int i = children.count()-1; i >= 0; --i) {
        if (QSGItem *child = qobject_cast<QSGItem *>(children.at(i))) {
            if (child->isVisible() && child->x() <= x
                && child->x() + child->width() >= x
                && child->y() <= y
                && child->y() + child->height() >= y)
                return child;
        }
    }
    return 0;
}

QDeclarativeListProperty<QObject> QSGItemPrivate::resources()
{
    return QDeclarativeListProperty<QObject>(q_func(), 0, QSGItemPrivate::resources_append,
                                             QSGItemPrivate::resources_count,
                                             QSGItemPrivate::resources_at,
                                             QSGItemPrivate::resources_clear);
}

QDeclarativeListProperty<QSGItem> QSGItemPrivate::children()
{
    return QDeclarativeListProperty<QSGItem>(q_func(), 0, QSGItemPrivate::children_append, 
                                             QSGItemPrivate::children_count,
                                             QSGItemPrivate::children_at,
                                             QSGItemPrivate::children_clear);
                                     
}

QDeclarativeListProperty<QDeclarativeState> QSGItemPrivate::states()
{
    return _states()->statesProperty();
}

QDeclarativeListProperty<QDeclarativeTransition> QSGItemPrivate::transitions()
{
    return _states()->transitionsProperty();
}

QString QSGItemPrivate::state() const
{
    if (!_stateGroup)
        return QString();
    else
        return _stateGroup->state();
}

void QSGItemPrivate::setState(const QString &state)
{
    _states()->setState(state);
}

QDeclarativeListProperty<QSGTransform> QSGItem::transform()
{
    Q_D(QSGItem);
    return QDeclarativeListProperty<QSGTransform>(this, 0, d->transform_append, d->transform_count,
                                                  d->transform_at, d->transform_clear);
}

void QSGItem::classBegin()
{
    Q_D(QSGItem);
    d->componentComplete = false;
    if (d->_stateGroup)
        d->_stateGroup->classBegin();
    if (d->_anchors)
        d->_anchors->classBegin();
}

void QSGItem::componentComplete()
{
    Q_D(QSGItem);
    d->componentComplete = true;
    if (d->_stateGroup)
        d->_stateGroup->componentComplete();
    if (d->_anchors) {
        d->_anchors->componentComplete();
        QSGAnchorsPrivate::get(d->_anchors)->updateOnComplete();
    }
    if (d->keyHandler)
        d->keyHandler->componentComplete();
    if (d->_contents)
        d->_contents->complete();
}

QDeclarativeStateGroup *QSGItemPrivate::_states()
{
    Q_Q(QSGItem);
    if (!_stateGroup) {
        _stateGroup = new QDeclarativeStateGroup;
        if (!componentComplete)
            _stateGroup->classBegin();
        QObject::connect(_stateGroup, SIGNAL(stateChanged(QString)),
                         q, SIGNAL(stateChanged(QString)));
    }

    return _stateGroup;
}

QSGItemPrivate::AnchorLines::AnchorLines(QSGItem *q)
{
    left.item = q;
    left.anchorLine = QSGAnchorLine::Left;
    right.item = q;
    right.anchorLine = QSGAnchorLine::Right;
    hCenter.item = q;
    hCenter.anchorLine = QSGAnchorLine::HCenter;
    top.item = q;
    top.anchorLine = QSGAnchorLine::Top;
    bottom.item = q;
    bottom.anchorLine = QSGAnchorLine::Bottom;
    vCenter.item = q;
    vCenter.anchorLine = QSGAnchorLine::VCenter;
    baseline.item = q;
    baseline.anchorLine = QSGAnchorLine::Baseline;
}

QPointF QSGItemPrivate::computeTransformOrigin() const
{
    switch(origin) {
    default:
    case QSGItem::TopLeft:
        return QPointF(0, 0);
    case QSGItem::Top:
        return QPointF(width / 2., 0);
    case QSGItem::TopRight:
        return QPointF(width, 0);
    case QSGItem::Left:
        return QPointF(0, height / 2.);
    case QSGItem::Center:
        return QPointF(width / 2., height / 2.);
    case QSGItem::Right:
        return QPointF(width, height / 2.);
    case QSGItem::BottomLeft:
        return QPointF(0, height);
    case QSGItem::Bottom:
        return QPointF(width / 2., height);
    case QSGItem::BottomRight:
        return QPointF(width, height);
    }
}

void QSGItemPrivate::transformChanged()
{
}

void QSGItemPrivate::deliverKeyEvent(QKeyEvent *e)
{
    Q_Q(QSGItem);
    
    Q_ASSERT(e->isAccepted());
    if (keyHandler) {
        if (e->type() == QEvent::KeyPress)
            keyHandler->keyPressed(e, false);
        else
            keyHandler->keyReleased(e, false);

        if (e->isAccepted())
            return;
        else
            e->accept();
    }

    if (e->type() == QEvent::KeyPress)
        q->keyPressEvent(e);
    else
        q->keyReleaseEvent(e);

    if (e->isAccepted())
        return;

    if (keyHandler) {
        e->accept();

        if (e->type() == QEvent::KeyPress)
            keyHandler->keyPressed(e, true);
        else
            keyHandler->keyReleased(e, true);
    }
}

void QSGItemPrivate::deliverInputMethodEvent(QInputMethodEvent *e)
{
    Q_Q(QSGItem);

    Q_ASSERT(e->isAccepted());
    if (keyHandler) {
        keyHandler->inputMethodEvent(e, false);

        if (e->isAccepted())
            return;
        else
            e->accept();
    }

    q->inputMethodEvent(e);

    if (e->isAccepted())
        return;

    if (keyHandler) {
        e->accept();

        keyHandler->inputMethodEvent(e, true);
    }
}

void QSGItemPrivate::deliverFocusEvent(QFocusEvent *e)
{
    Q_Q(QSGItem);

    if (e->type() == QEvent::FocusIn) {
        q->focusInEvent(e);
    } else {
        q->focusOutEvent(e);
    }
}

void QSGItemPrivate::deliverMouseEvent(QGraphicsSceneMouseEvent *e)
{
    Q_Q(QSGItem);

    Q_ASSERT(e->isAccepted());

    switch(e->type()) {
    default:
        Q_ASSERT(!"Unknown event type");
    case QEvent::GraphicsSceneMouseMove:
        q->mouseMoveEvent(e);
        break;
    case QEvent::GraphicsSceneMousePress:
        q->mousePressEvent(e);
        break;
    case QEvent::GraphicsSceneMouseRelease:
        q->mouseReleaseEvent(e);
        break;
    case QEvent::GraphicsSceneMouseDoubleClick:
        q->mouseDoubleClickEvent(e);
        break;
    }
}

void QSGItemPrivate::deliverWheelEvent(QGraphicsSceneWheelEvent *e)
{
    Q_Q(QSGItem);
    q->wheelEvent(e);
}

void QSGItemPrivate::deliverTouchEvent(QTouchEvent *e)
{
    Q_Q(QSGItem);
    q->touchEvent(e);
}

void QSGItemPrivate::deliverHoverEvent(QGraphicsSceneHoverEvent *e)
{
    Q_Q(QSGItem);
    switch(e->type()) {
    default:
        Q_ASSERT(!"Unknown event type");
    case QEvent::GraphicsSceneHoverEnter:
        q->hoverEnterEvent(e);
        break;
    case QEvent::GraphicsSceneHoverLeave:
        q->hoverLeaveEvent(e);
        break;
    case QEvent::GraphicsSceneHoverMove:
        q->hoverMoveEvent(e);
        break;
    }
}

void QSGItem::itemChange(ItemChange change, const ItemChangeData &value)
{
    Q_UNUSED(change);
    Q_UNUSED(value);
}

/*! \internal */
// XXX todo - do we want/need this anymore?
QRectF QSGItem::boundingRect() const
{
    Q_D(const QSGItem);
    return QRectF(0, 0, d->width, d->height);
}

QSGItem::TransformOrigin QSGItem::transformOrigin() const
{
    Q_D(const QSGItem);
    return d->origin;
}

void QSGItem::setTransformOrigin(TransformOrigin origin)
{
    Q_D(QSGItem);
    if (origin == d->origin) 
        return;

    d->origin = origin;
    d->dirty(QSGItemPrivate::TransformOrigin);

    emit transformOriginChanged(d->origin);
}

QPointF QSGItem::transformOriginPoint() const
{
    Q_D(const QSGItem);
    return d->computeTransformOrigin();
}

qreal QSGItem::z() const
{
    Q_D(const QSGItem);
    return d->z;
}

void QSGItem::setZ(qreal v)
{
    Q_D(QSGItem);
    if (d->z == v)
        return;

    d->z = v;

    d->dirty(QSGItemPrivate::ZValue);
    if (d->parentItem) 
        QSGItemPrivate::get(d->parentItem)->dirty(QSGItemPrivate::ChildrenStackingChanged);

    emit zChanged();
}

qreal QSGItem::rotation() const 
{ 
    Q_D(const QSGItem);
    return d->rotation;
}

void QSGItem::setRotation(qreal r) 
{
    Q_D(QSGItem);
    if (d->rotation == r)
        return;

    d->rotation = r;

    d->dirty(QSGItemPrivate::BasicTransform);

    d->itemChange(ItemRotationHasChanged, r);

    emit rotationChanged();
}

qreal QSGItem::scale() const 
{ 
    Q_D(const QSGItem);
    return d->scale;
}

void QSGItem::setScale(qreal s) 
{
    Q_D(QSGItem);
    if (d->scale == s)
        return;

    d->scale = s;

    d->dirty(QSGItemPrivate::BasicTransform);

    emit scaleChanged();
}

qreal QSGItem::opacity() const
{
    Q_D(const QSGItem);
    return d->opacity;
}

void QSGItem::setOpacity(qreal o)
{
    Q_D(QSGItem);
    if (d->opacity == o)
        return;

    d->opacity = o;
    
    d->dirty(QSGItemPrivate::OpacityValue);

    d->itemChange(ItemOpacityHasChanged, o);

    emit opacityChanged();
}

bool QSGItem::isVisible() const
{
    Q_D(const QSGItem);
    return d->effectiveVisible;
}

void QSGItem::setVisible(bool v)
{
    Q_D(QSGItem);
    if (v == d->explicitVisible)
        return;

    d->explicitVisible = v;

    d->setEffectiveVisibleRecur(d->calcEffectiveVisible());
}

bool QSGItem::isEnabled() const
{
    Q_D(const QSGItem);
    return d->effectiveEnable;
}

void QSGItem::setEnabled(bool e)
{
    Q_D(QSGItem);
    if (e == d->explicitEnable)
        return;

    d->explicitEnable = e;

    d->setEffectiveEnableRecur(d->calcEffectiveEnable());
}

bool QSGItemPrivate::calcEffectiveVisible() const
{
    // XXX todo - Should the effective visible of an element with no parent just be the current
    // effective visible?  This would prevent pointless re-processing in the case of an element
    // moving to/from a no-parent situation, but it is different from what graphics view does.
    return explicitVisible && (!parentItem || QSGItemPrivate::get(parentItem)->effectiveVisible);
}

void QSGItemPrivate::setEffectiveVisibleRecur(bool newEffectiveVisible)
{
    Q_Q(QSGItem);

    if (newEffectiveVisible && !explicitVisible) {
        // This item locally overrides visibility
        return;
    }

    if (newEffectiveVisible == effectiveVisible) {
        // No change necessary
        return;
    }

    effectiveVisible = newEffectiveVisible;
    dirty(Visible);
    if (parentItem) QSGItemPrivate::get(parentItem)->dirty(ChildrenStackingChanged);

    if (canvas) {
        QSGCanvasPrivate *canvasPriv = QSGCanvasPrivate::get(canvas);
        if (canvasPriv->mouseGrabberItem == q)
            q->ungrabMouse();
    }

    for (int ii = 0; ii < childItems.count(); ++ii) 
        QSGItemPrivate::get(childItems.at(ii))->setEffectiveVisibleRecur(newEffectiveVisible);

    for(int ii = 0; ii < changeListeners.count(); ++ii) {
        const QSGItemPrivate::ChangeListener &change = changeListeners.at(ii);
        if (change.types & QSGItemPrivate::Visibility)
            change.listener->itemVisibilityChanged(q);
    }

    emit q->visibleChanged();
}

bool QSGItemPrivate::calcEffectiveEnable() const
{
    // XXX todo - Should the effective enable of an element with no parent just be the current
    // effective enable?  This would prevent pointless re-processing in the case of an element
    // moving to/from a no-parent situation, but it is different from what graphics view does.
    return explicitEnable && (!parentItem || QSGItemPrivate::get(parentItem)->effectiveEnable);
}

void QSGItemPrivate::setEffectiveEnableRecur(bool newEffectiveEnable)
{
    Q_Q(QSGItem);

    // XXX todo - need to fixup focus

    if (newEffectiveEnable && !explicitEnable) {
        // This item locally overrides enable
        return;
    }

    if (newEffectiveEnable == effectiveEnable) {
        // No change necessary
        return;
    }

    effectiveEnable = newEffectiveEnable;

    if (canvas) {
        QSGCanvasPrivate *canvasPriv = QSGCanvasPrivate::get(canvas);
        if (canvasPriv->mouseGrabberItem == q)
            q->ungrabMouse();
    }

    for (int ii = 0; ii < childItems.count(); ++ii) 
        QSGItemPrivate::get(childItems.at(ii))->setEffectiveEnableRecur(newEffectiveEnable);

    emit q->enabledChanged();
}

QString QSGItemPrivate::dirtyToString() const
{
#define DIRTY_TO_STRING(value) if (dirtyAttributes & value) { \
    if (!rv.isEmpty()) \
        rv.append(QLatin1String("|")); \
    rv.append(QLatin1String(#value)); \
}

//    QString rv = QLatin1String("0x") + QString::number(dirtyAttributes, 16);
    QString rv;

    DIRTY_TO_STRING(TransformOrigin);
    DIRTY_TO_STRING(Transform);
    DIRTY_TO_STRING(BasicTransform);
    DIRTY_TO_STRING(Position);
    DIRTY_TO_STRING(Size);
    DIRTY_TO_STRING(ZValue);
    DIRTY_TO_STRING(Content);
    DIRTY_TO_STRING(Smooth);
    DIRTY_TO_STRING(OpacityValue);
    DIRTY_TO_STRING(ChildrenChanged);
    DIRTY_TO_STRING(ChildrenStackingChanged);
    DIRTY_TO_STRING(ParentChanged);
    DIRTY_TO_STRING(Clip);
    DIRTY_TO_STRING(Canvas);
    DIRTY_TO_STRING(EffectReference);
    DIRTY_TO_STRING(Visible);
    DIRTY_TO_STRING(HideReference);

    return rv;
}

void QSGItemPrivate::dirty(DirtyType type)
{
    Q_Q(QSGItem);
    if (type & (TransformOrigin | Transform | BasicTransform | Position | Size))
        transformChanged();

    if (!(dirtyAttributes & type) || (canvas && !prevDirtyItem)) {
        dirtyAttributes |= type;
        if (canvas) {
            addToDirtyList();
            QSGCanvasPrivate::get(canvas)->dirtyItem(q);
        }
    }
}

void QSGItemPrivate::addToDirtyList()
{
    Q_Q(QSGItem);

    Q_ASSERT(canvas);
    if (!prevDirtyItem) {
        Q_ASSERT(!nextDirtyItem);

        QSGCanvasPrivate *p = QSGCanvasPrivate::get(canvas);
        nextDirtyItem = p->dirtyItemList;
        if (nextDirtyItem) QSGItemPrivate::get(nextDirtyItem)->prevDirtyItem = &nextDirtyItem;
        prevDirtyItem = &p->dirtyItemList;
        p->dirtyItemList = q;
        p->dirtyItem(q);
    }
    Q_ASSERT(prevDirtyItem);
}

void QSGItemPrivate::removeFromDirtyList()
{
    if (prevDirtyItem) {
        if (nextDirtyItem) QSGItemPrivate::get(nextDirtyItem)->prevDirtyItem = prevDirtyItem;
        *prevDirtyItem = nextDirtyItem;
        prevDirtyItem = 0;
        nextDirtyItem = 0;
    }
    Q_ASSERT(!prevDirtyItem);
    Q_ASSERT(!nextDirtyItem);
}

void QSGItemPrivate::refFromEffectItem(bool hide)
{
    ++effectRefCount;
    if (1 == effectRefCount) {
        dirty(EffectReference);
        if (parentItem) QSGItemPrivate::get(parentItem)->dirty(ChildrenStackingChanged);
    }
    if (hide) {
        if (++hideRefCount == 1)
            dirty(HideReference);
    }
}

void QSGItemPrivate::derefFromEffectItem(bool unhide)
{
    Q_ASSERT(effectRefCount);
    --effectRefCount;
    if (0 == effectRefCount) {
        dirty(EffectReference);
        if (parentItem) QSGItemPrivate::get(parentItem)->dirty(ChildrenStackingChanged);
    }
    if (unhide) {
        if (--hideRefCount == 0)
            dirty(HideReference);
    }
}

void QSGItemPrivate::itemChange(QSGItem::ItemChange change, const QSGItem::ItemChangeData &data)
{
    Q_Q(QSGItem);
    switch(change) {
    case QSGItem::ItemChildAddedChange:
        q->itemChange(change, data);
        if (_contents && componentComplete)
            _contents->childAdded(data.item);
        for(int ii = 0; ii < changeListeners.count(); ++ii) {
            const QSGItemPrivate::ChangeListener &change = changeListeners.at(ii);
            if (change.types & QSGItemPrivate::Children) {
                change.listener->itemChildAdded(q, data.item);
            }
        }
        break;
    case QSGItem::ItemChildRemovedChange:
        q->itemChange(change, data);
        if (_contents && componentComplete)
            _contents->childRemoved(data.item);
        for(int ii = 0; ii < changeListeners.count(); ++ii) {
            const QSGItemPrivate::ChangeListener &change = changeListeners.at(ii);
            if (change.types & QSGItemPrivate::Children) {
                change.listener->itemChildRemoved(q, data.item);
            }
        }
        break;
    case QSGItem::ItemSceneChange:
        q->itemChange(change, data);
        break;
    case QSGItem::ItemVisibleHasChanged:
        q->itemChange(change, data);
        for(int ii = 0; ii < changeListeners.count(); ++ii) {
            const QSGItemPrivate::ChangeListener &change = changeListeners.at(ii);
            if (change.types & QSGItemPrivate::Visibility) {
                change.listener->itemVisibilityChanged(q);
            }
        }
        break;
    case QSGItem::ItemParentHasChanged:
        q->itemChange(change, data);
        for(int ii = 0; ii < changeListeners.count(); ++ii) {
            const QSGItemPrivate::ChangeListener &change = changeListeners.at(ii);
            if (change.types & QSGItemPrivate::Parent) {
                change.listener->itemParentChanged(q, data.item);
            }
        }
        break;
    case QSGItem::ItemOpacityHasChanged:
        q->itemChange(change, data);
        for(int ii = 0; ii < changeListeners.count(); ++ii) {
            const QSGItemPrivate::ChangeListener &change = changeListeners.at(ii);
            if (change.types & QSGItemPrivate::Opacity) {
                change.listener->itemOpacityChanged(q);
            }
        }
        break;
    case QSGItem::ItemActiveFocusHasChanged:
        q->itemChange(change, data);
        break;
    case QSGItem::ItemRotationHasChanged:
        q->itemChange(change, data);
        for(int ii = 0; ii < changeListeners.count(); ++ii) {
            const QSGItemPrivate::ChangeListener &change = changeListeners.at(ii);
            if (change.types & QSGItemPrivate::Rotation) {
                change.listener->itemRotationChanged(q);
            }
        }
        break;
    }
}

bool QSGItem::smooth() const
{
    Q_D(const QSGItem);
    return d->smooth;
}

void QSGItem::setSmooth(bool smooth)
{
    Q_D(QSGItem);
    if (d->smooth == smooth)
        return;

    d->smooth = smooth;
    d->dirty(QSGItemPrivate::Smooth);

    emit smoothChanged(smooth);
}

QSGItem::Flags QSGItem::flags() const 
{ 
    Q_D(const QSGItem);
    return (QSGItem::Flags)d->flags;
}

void QSGItem::setFlag(Flag flag, bool enabled)
{ 
    Q_D(QSGItem);
    if (enabled)
        setFlags((Flags)(d->flags | (quint32)flag));
    else
        setFlags((Flags)(d->flags & ~(quint32)flag));
}

void QSGItem::setFlags(Flags flags) 
{ 
    Q_D(QSGItem);

    if ((flags & ItemIsFocusScope) != (d->flags & ItemIsFocusScope)) {
        if (flags & ItemIsFocusScope && !d->childItems.isEmpty() && d->canvas) {
            qWarning("QSGItem: Cannot set FocusScope once item has children and is in a canvas.");
            flags &= ~ItemIsFocusScope;
        } else if (d->flags & ItemIsFocusScope) {
            qWarning("QSGItem: Cannot unset FocusScope flag.");
            flags |= ItemIsFocusScope;
        } 
    }

    if ((flags & ItemClipsChildrenToShape ) != (d->flags & ItemClipsChildrenToShape)) 
        d->dirty(QSGItemPrivate::Clip);

    d->flags = flags;
}

qreal QSGItem::x() const
{
    Q_D(const QSGItem);
    return d->x;
}

qreal QSGItem::y() const
{
    Q_D(const QSGItem);
    return d->y;
}

QPointF QSGItem::pos() const
{
    Q_D(const QSGItem);
    return QPointF(d->x, d->y);
}

void QSGItem::setX(qreal v)
{
    Q_D(QSGItem);
    if (d->x == v)
        return;

    qreal oldx = d->x;
    d->x = v;

    d->dirty(QSGItemPrivate::Position);

    geometryChanged(QRectF(x(), y(), width(), height()),
                    QRectF(oldx, y(), width(), height()));
}

void QSGItem::setY(qreal v)
{
    Q_D(QSGItem);
    if (d->y == v)
        return;

    qreal oldy = d->y;
    d->y = v;

    d->dirty(QSGItemPrivate::Position);

    geometryChanged(QRectF(x(), y(), width(), height()),
                    QRectF(x(), oldy, width(), height()));
}

void QSGItem::setPos(const QPointF &pos)
{
    Q_D(QSGItem);
    if (QPointF(d->x, d->y) == pos)
        return;

    qreal oldx = d->x;
    qreal oldy = d->y;

    d->x = pos.x();
    d->y = pos.y();

    d->dirty(QSGItemPrivate::Position);

    geometryChanged(QRectF(x(), y(), width(), height()),
                    QRectF(oldx, oldy, width(), height()));
}

qreal QSGItem::width() const
{
    Q_D(const QSGItem);
    return d->width;
}

void QSGItem::setWidth(qreal w)
{
    Q_D(QSGItem);
    if (qIsNaN(w))
        return;

    d->widthValid = true;
    if (d->width == w)
        return;

    qreal oldWidth = d->width;
    d->width = w;

    d->dirty(QSGItemPrivate::Size);

    geometryChanged(QRectF(x(), y(), width(), height()),
                    QRectF(x(), y(), oldWidth, height()));
}

void QSGItem::resetWidth()
{
    Q_D(QSGItem);
    d->widthValid = false;
    setImplicitWidth(implicitWidth());
}

void QSGItemPrivate::implicitWidthChanged()
{
    Q_Q(QSGItem);
    emit q->implicitWidthChanged();
}

qreal QSGItemPrivate::getImplicitWidth() const
{
    return implicitWidth;
}

qreal QSGItem::implicitWidth() const
{
    Q_D(const QSGItem);
    return d->getImplicitWidth();
}

void QSGItem::setImplicitWidth(qreal w)
{
    Q_D(QSGItem);
    bool changed = w != d->implicitWidth;
    d->implicitWidth = w;
    if (d->width == w || widthValid()) {
        if (changed)
            d->implicitWidthChanged();
        return;
    }   

    qreal oldWidth = d->width;
    d->width = w;
    
    d->dirty(QSGItemPrivate::Size);

    geometryChanged(QRectF(x(), y(), width(), height()),
                    QRectF(x(), y(), oldWidth, height()));

    if (changed)
        d->implicitWidthChanged();
}

bool QSGItem::widthValid() const
{
    Q_D(const QSGItem);
    return d->widthValid;
}

qreal QSGItem::height() const
{
    Q_D(const QSGItem);
    return d->height;
}

void QSGItem::setHeight(qreal h)
{
    Q_D(QSGItem);
    if (qIsNaN(h))
        return;

    d->heightValid = true;
    if (d->height == h)
        return;

    qreal oldHeight = d->height;
    d->height = h;

    d->dirty(QSGItemPrivate::Size);

    geometryChanged(QRectF(x(), y(), width(), height()),
                    QRectF(x(), y(), width(), oldHeight));
}

void QSGItem::resetHeight()
{
    Q_D(QSGItem);
    d->heightValid = false;
    setImplicitHeight(implicitHeight());
}

void QSGItemPrivate::implicitHeightChanged()
{
    Q_Q(QSGItem);
    emit q->implicitHeightChanged();
}

qreal QSGItemPrivate::getImplicitHeight() const
{
    return implicitHeight;
}

qreal QSGItem::implicitHeight() const
{
    Q_D(const QSGItem);
    return d->getImplicitHeight();
}

void QSGItem::setImplicitHeight(qreal h)
{
    Q_D(QSGItem);
    bool changed = h != d->implicitHeight;
    d->implicitHeight = h;
    if (d->height == h || heightValid()) {
        if (changed)
            d->implicitHeightChanged();
        return;
    }

    qreal oldHeight = d->height;
    d->height = h;

    d->dirty(QSGItemPrivate::Size);

    geometryChanged(QRectF(x(), y(), width(), height()),
                    QRectF(x(), y(), width(), oldHeight));

    if (changed)
        d->implicitHeightChanged();
}

bool QSGItem::heightValid() const
{
    Q_D(const QSGItem);
    return d->heightValid;
}

void QSGItem::setSize(const QSizeF &size)
{
    Q_D(QSGItem);
    d->heightValid = true;
    d->widthValid = true;

    if (QSizeF(d->width, d->height) == size)
        return;

    qreal oldHeight = d->height;
    qreal oldWidth = d->width;
    d->height = size.height();
    d->width = size.width();

    d->dirty(QSGItemPrivate::Size);

    geometryChanged(QRectF(x(), y(), width(), height()),
                    QRectF(x(), y(), oldWidth, oldHeight));
}

bool QSGItem::hasActiveFocus() const
{
    Q_D(const QSGItem);
    return d->activeFocus;
}

bool QSGItem::hasFocus() const
{
    Q_D(const QSGItem);
    return d->focus;
}

void QSGItem::setFocus(bool focus)
{
    Q_D(QSGItem);
    if (d->focus == focus)
        return;

    if (d->canvas) {
        // Need to find our nearest focus scope
        QSGItem *scope = parentItem();
        while (scope && !scope->isFocusScope())
            scope = scope->parentItem();
        if (focus)
            QSGCanvasPrivate::get(d->canvas)->setFocusInScope(scope, this);
        else
            QSGCanvasPrivate::get(d->canvas)->clearFocusInScope(scope, this);
    } else {
        d->focus = focus;
        emit focusChanged(focus);
    }
}

bool QSGItem::isFocusScope() const
{
    return flags() & ItemIsFocusScope;
}

QSGItem *QSGItem::scopedFocusItem() const
{
    Q_D(const QSGItem);
    if (!isFocusScope())
        return 0;
    else 
        return d->subFocusItem;
}


Qt::MouseButtons QSGItem::acceptedMouseButtons() const 
{ 
    Q_D(const QSGItem);
    return d->acceptedMouseButtons;
}

void QSGItem::setAcceptedMouseButtons(Qt::MouseButtons buttons) 
{ 
    Q_D(QSGItem);
    d->acceptedMouseButtons = buttons;
}

bool QSGItem::filtersChildMouseEvents() const
{
    Q_D(const QSGItem);
    return d->filtersChildMouseEvents;
}

void QSGItem::setFiltersChildMouseEvents(bool filter) 
{ 
    Q_D(QSGItem);
    d->filtersChildMouseEvents = filter;
}

bool QSGItem::isUnderMouse() const 
{
    Q_D(const QSGItem);
    if (!d->canvas)
        return false;

    QPoint cursorPos = QCursor::pos();
    if (QRectF(0, 0, width(), height()).contains(mapFromScene(d->canvas->mapFromGlobal(cursorPos))))
        return true;
    return false; 
}

bool QSGItem::acceptHoverEvents() const 
{ 
    Q_D(const QSGItem);
    return d->hoverEnabled;
}

void QSGItem::setAcceptHoverEvents(bool enabled) 
{ 
    Q_D(QSGItem);
    d->hoverEnabled = enabled;

    if (d->canvas && d->hoverEnabled && !d->canvas->hasMouseTracking())
        d->canvas->setMouseTracking(true);
}

void QSGItem::grabMouse() 
{
    Q_D(QSGItem);
    if (!d->canvas)
        return;
    QSGCanvasPrivate *canvasPriv = QSGCanvasPrivate::get(d->canvas);
    if (canvasPriv->mouseGrabberItem == this)
        return;

    QSGItem *oldGrabber = canvasPriv->mouseGrabberItem;
    canvasPriv->mouseGrabberItem = this;
    if (oldGrabber)
        oldGrabber->mouseUngrabEvent();
}

void QSGItem::ungrabMouse() 
{
    Q_D(QSGItem);
    if (!d->canvas)
        return;
    QSGCanvasPrivate *canvasPriv = QSGCanvasPrivate::get(d->canvas);
    if (canvasPriv->mouseGrabberItem != this) {
        qWarning("QSGItem::ungrabMouse(): Item is not the mouse grabber.");
        return;
    }

    canvasPriv->mouseGrabberItem = 0;
    mouseUngrabEvent();
}

bool QSGItem::keepMouseGrab() const
{
    Q_D(const QSGItem);
    return d->keepMouse;
}

void QSGItem::setKeepMouseGrab(bool keep)
{
    Q_D(QSGItem);
    d->keepMouse = keep;
}

QPointF QSGItem::mapToItem(const QSGItem *item, const QPointF &point) const 
{ 
    QPointF p = mapToScene(point);
    if (item)
        p = item->mapFromScene(p);
    return p;
}

QPointF QSGItem::mapToScene(const QPointF &point) const 
{ 
    Q_D(const QSGItem);
    return d->itemToCanvasTransform().map(point);
}

QRectF QSGItem::mapRectToItem(const QSGItem *item, const QRectF &rect) const
{
    Q_D(const QSGItem);
    QTransform t = d->itemToCanvasTransform();
    if (item)
        t *= QSGItemPrivate::get(item)->canvasToItemTransform();
    return t.mapRect(rect);
}

QRectF QSGItem::mapRectToScene(const QRectF &rect) const 
{ 
    Q_D(const QSGItem);
    return d->itemToCanvasTransform().mapRect(rect);
}

QPointF QSGItem::mapFromItem(const QSGItem *item, const QPointF &point) const 
{ 
    QPointF p = item?item->mapToScene(point):point;
    return mapFromScene(p);
}

QPointF QSGItem::mapFromScene(const QPointF &point) const 
{ 
    Q_D(const QSGItem);
    return d->canvasToItemTransform().map(point);
}

QRectF QSGItem::mapRectFromItem(const QSGItem *item, const QRectF &rect) const 
{ 
    Q_D(const QSGItem);
    QTransform t = item?QSGItemPrivate::get(item)->itemToCanvasTransform():QTransform();
    t *= d->canvasToItemTransform();
    return t.mapRect(rect);
}

QRectF QSGItem::mapRectFromScene(const QRectF &rect) const
{
    Q_D(const QSGItem);
    return d->canvasToItemTransform().mapRect(rect);
}

bool QSGItem::event(QEvent *ev)
{
    return QObject::event(ev);

#if 0
    if (ev->type() == QEvent::PolishRequest) {
        Q_D(QSGItem);
        d->polishScheduled = false;
        updatePolish();
        return true;
    } else {
        return QObject::event(ev);
    }
#endif
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, QSGItem *item)
{
    if (!item) {
        debug << "QSGItem(0)";
        return debug;
    }

    debug << item->metaObject()->className() << "(this =" << ((void*)item)
          << ", name=" << item->objectName()
          << ", parent =" << ((void*)item->parentItem())
          << ", geometry =" << QRectF(item->pos(), QSizeF(item->width(), item->height()))
          << ", z =" << item->z() << ')';
    return debug;
}
#endif

qint64 QSGItemPrivate::consistentTime = -1;
void QSGItemPrivate::setConsistentTime(qint64 t)
{
    consistentTime = t;
}

class QElapsedTimerConsistentTimeHack
{
public:
    void start() {
        t1 = QSGItemPrivate::consistentTime;
        t2 = 0;
    }
    qint64 elapsed() {
        return QSGItemPrivate::consistentTime - t1;
    }
    qint64 restart() {
        qint64 val = QSGItemPrivate::consistentTime - t1;
        t1 = QSGItemPrivate::consistentTime;
        t2 = 0;
        return val;
    }

private:
    qint64 t1;
    qint64 t2;
};

void QSGItemPrivate::start(QElapsedTimer &t)
{
    if (QSGItemPrivate::consistentTime == -1)
        t.start();
    else
        ((QElapsedTimerConsistentTimeHack*)&t)->start();
}

qint64 QSGItemPrivate::elapsed(QElapsedTimer &t)
{
    if (QSGItemPrivate::consistentTime == -1)
        return t.elapsed();
    else
        return ((QElapsedTimerConsistentTimeHack*)&t)->elapsed();
}

qint64 QSGItemPrivate::restart(QElapsedTimer &t)
{
    if (QSGItemPrivate::consistentTime == -1)
        return t.restart();
    else
        return ((QElapsedTimerConsistentTimeHack*)&t)->restart();
}

QT_END_NAMESPACE

#include <moc_qsgitem.cpp>
