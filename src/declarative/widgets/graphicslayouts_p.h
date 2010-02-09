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

#ifndef GRAPHICSLAYOUTS_H
#define GRAPHICSLAYOUTS_H

#include "graphicswidgets_p.h"

#include <QtGui/QGraphicsLinearLayout>
#include <QtGui/QGraphicsGridLayout>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QGraphicsLinearLayoutStretchItemObject : public QObject, public QGraphicsLayoutItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsLayoutItem)
public:
    QGraphicsLinearLayoutStretchItemObject(QObject *parent = 0);

    virtual QSizeF sizeHint(Qt::SizeHint, const QSizeF &) const;
};

class LinearLayoutAttached;
class QGraphicsLinearLayoutObject : public QObject, public QGraphicsLinearLayout
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsLayout QGraphicsLayoutItem)

    Q_PROPERTY(QmlList<QGraphicsLayoutItem *> *children READ children)
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
    Q_PROPERTY(qreal spacing READ spacing WRITE setSpacing)
    Q_CLASSINFO("DefaultProperty", "children")
public:
    QGraphicsLinearLayoutObject(QObject * = 0);
    ~QGraphicsLinearLayoutObject();

    QmlList<QGraphicsLayoutItem *> *children() { return &_children; }

    static LinearLayoutAttached *qmlAttachedProperties(QObject *);

private Q_SLOTS:
    void updateStretch(QGraphicsLayoutItem*,int);
    void updateAlignment(QGraphicsLayoutItem*,Qt::Alignment);

private:
    friend class LinearLayoutAttached;
    void clearChildren();
    void insertLayoutItem(int, QGraphicsLayoutItem *);
    static QHash<QGraphicsLayoutItem*, LinearLayoutAttached*> attachedProperties;

    class ChildList : public QmlList<QGraphicsLayoutItem *>
    {
    public:
        ChildList(QGraphicsLinearLayoutObject *o)
            : obj(o) {}

        virtual void append(QGraphicsLayoutItem *item)
        {
            insert(-1, item);
        }
        virtual void clear() { obj->clearChildren(); }
        virtual int count() const { return obj->count(); }
        virtual void removeAt(int i) { obj->removeAt(i); }
        virtual QGraphicsLayoutItem *at(int i) const { return obj->itemAt(i); }
        virtual void insert(int i, QGraphicsLayoutItem *item) { obj->insertLayoutItem(i, item); }

    private:
        QGraphicsLinearLayoutObject *obj;
    };

    ChildList _children;
};

class GridLayoutAttached;
class QGraphicsGridLayoutObject : public QObject, public QGraphicsGridLayout
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsLayout QGraphicsLayoutItem)

    Q_PROPERTY(QmlList<QGraphicsLayoutItem *> *children READ children)
    Q_PROPERTY(qreal spacing READ spacing WRITE setSpacing)
    Q_PROPERTY(qreal verticalSpacing READ verticalSpacing WRITE setVerticalSpacing)
    Q_PROPERTY(qreal horizontalSpacing READ horizontalSpacing WRITE setHorizontalSpacing)
    Q_CLASSINFO("DefaultProperty", "children")
public:
    QGraphicsGridLayoutObject(QObject * = 0);
    ~QGraphicsGridLayoutObject();

    QmlList<QGraphicsLayoutItem *> *children() { return &_children; }

    qreal spacing() const;

    static GridLayoutAttached *qmlAttachedProperties(QObject *);

private:
    friend class GraphicsLayoutAttached;
    void addWidget(QGraphicsWidget *);
    void clearChildren();
    void addLayoutItem(QGraphicsLayoutItem *);
    static QHash<QGraphicsLayoutItem*, GridLayoutAttached*> attachedProperties;

    class ChildList : public QmlList<QGraphicsLayoutItem *>
    {
    public:
        ChildList(QGraphicsGridLayoutObject *o)
            : obj(o) {}

        virtual void append(QGraphicsLayoutItem *o)
        {
            obj->addLayoutItem(o);
        }
        virtual void clear() { obj->clearChildren(); }
        virtual int count() const { return obj->count(); }
        virtual void removeAt(int i) { obj->removeAt(i); }
        virtual QGraphicsLayoutItem *at(int i) const { return obj->itemAt(i); }
        //### GridLayout doesn't have an insert, so for now we treat it as an append.
        //    this is obviously potenitally dangerous -- perhaps should be a concrete
        //    list with no relation to layout index, etc at all.
        virtual void insert(int, QGraphicsLayoutItem *item) { append(item); }

    private:
        QGraphicsGridLayoutObject *obj;
    };

    ChildList _children;
};

QT_END_NAMESPACE

QML_DECLARE_INTERFACE(QGraphicsLayoutItem)
QML_DECLARE_INTERFACE(QGraphicsLayout)
QML_DECLARE_TYPE(QGraphicsLinearLayoutStretchItemObject)
QML_DECLARE_TYPE(QGraphicsLinearLayoutObject)
QML_DECLARE_TYPEINFO(QGraphicsLinearLayoutObject, QML_HAS_ATTACHED_PROPERTIES)
QML_DECLARE_TYPE(QGraphicsGridLayoutObject)
QML_DECLARE_TYPEINFO(QGraphicsGridLayoutObject, QML_HAS_ATTACHED_PROPERTIES)

QT_END_HEADER

#endif // GRAPHICSLAYOUTS_H
