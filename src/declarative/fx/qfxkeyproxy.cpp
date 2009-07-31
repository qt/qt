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

#include "qfxkeyproxy.h"
#include <QGraphicsScene>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,KeyProxy,QFxKeyProxy)

/*!
    \qmlclass KeyProxy
    \brief The KeyProxy item proxies key events to a number of other items.
    \inherits Item

    KeyProxy provides a way to forward key presses, key releases, and keyboard input
    coming from input methods to other items. This can be useful when you want
    one item to handle some keys (e.g. the up and down arrow keys), and another item to
    handle other keys (e.g. the left and right arrow keys).

    To receive key events, the KeyProxy item must be in the current focus chain,
    just like any other item.

    KeyProxy is an invisible item: it is never painted.

    A simple KeyProxy that forward key events to two lists:
    \qml
    ListView { id: List1 ... }
    ListView { id: List2 ... }
    KeyProxy {
        focus: true
        targets: [List1, List2]
    }
    \endqml
*/

/*!
    \internal
    \class QFxKeyProxy
    \brief The QFxKeyProxy class proxies key events to a number of other items.
*/

class QFxKeyProxyPrivate
{
public:
    QFxKeyProxyPrivate() : inPress(false), inRelease(false), inIM(false), imeItem(0) {}
    QList<QFxItem *> targets;

    QGraphicsItem *finalFocusProxy(QGraphicsItem *item)
    {
        QGraphicsItem *fp;
        while ((fp = item->focusProxy()))
            item = fp;
        return item;
    }

    //loop detection
    bool inPress:1;
    bool inRelease:1;
    bool inIM:1;

    QGraphicsItem *imeItem;
};

QFxKeyProxy::QFxKeyProxy(QFxItem *parent)
: QFxItem(parent), d(new QFxKeyProxyPrivate)
{
    setFlag(QGraphicsItem::ItemAcceptsInputMethod);
}

QFxKeyProxy::~QFxKeyProxy()
{
    delete d; d = 0;
}

/*!
    \qmlproperty list<Item> KeyProxy::targets

    An ordered list of the items that will be forwarded key events.

    The events will be forwarded to the targets in turn until one of them
    accepts the event.
*/

QList<QFxItem *> *QFxKeyProxy::targets() const
{
    return &d->targets;
}

void QFxKeyProxy::keyPressEvent(QKeyEvent *e)
{
    if (!scene())
        return;
    if (!d->inPress) {
        d->inPress = true;
        for (int ii = 0; ii < d->targets.count(); ++ii) {
            QGraphicsItem *i = d->finalFocusProxy(d->targets.at(ii));
            if (i) {
                scene()->sendEvent(i, e);
                if (e->isAccepted()) {
                    d->inPress = false;
                    return;
                }
            }
        }
        d->inPress = false;
    }
}

void QFxKeyProxy::keyReleaseEvent(QKeyEvent *e)
{
    if (!scene())
        return;
    if (!d->inRelease) {
        d->inRelease = true;
        for (int ii = 0; ii < d->targets.count(); ++ii) {
            QGraphicsItem *i = d->finalFocusProxy(d->targets.at(ii));
            if (i) {
                scene()->sendEvent(i, e);
                if (e->isAccepted()) {
                    d->inRelease = false;
                    return;
                }
            }
        }
        d->inRelease = false;
    }
}

void QFxKeyProxy::inputMethodEvent(QInputMethodEvent *e)
{
    if (!scene())
        return;
    if (!d->inIM) {
        d->inIM = true;
        for (int ii = 0; ii < d->targets.count(); ++ii) {
            QGraphicsItem *i = d->finalFocusProxy(d->targets.at(ii));
            if (i && (i->flags() & ItemAcceptsInputMethod)) {
                scene()->sendEvent(i, e);
                if (e->isAccepted()) {
                    d->imeItem = i;
                    d->inIM = false;
                    return;
                }
            }
        }
        d->inIM = false;
    }
}

class QFxItemAccessor : public QGraphicsItem
{
public:
    QVariant doInputMethodQuery(Qt::InputMethodQuery query) const {
        return QGraphicsItem::inputMethodQuery(query);
    }
};

QVariant QFxKeyProxy::inputMethodQuery(Qt::InputMethodQuery query) const
{   
    for (int ii = 0; ii < d->targets.count(); ++ii) {
            QGraphicsItem *i = d->finalFocusProxy(d->targets.at(ii));
        if (i && (i->flags() & ItemAcceptsInputMethod) && i == d->imeItem) { //### how robust is i == d->imeItem check?
            QVariant v = static_cast<QFxItemAccessor *>(i)->doInputMethodQuery(query);
            if (v.type() == QVariant::RectF)
                v = mapRectFromItem(i, v.toRectF());  //### cost?
            return v;
        }
    }
    return QFxItem::inputMethodQuery(query);
}

QT_END_NAMESPACE
