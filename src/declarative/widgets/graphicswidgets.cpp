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

#include "graphicswidgets_p.h"

QT_BEGIN_NAMESPACE

class QGraphicsViewDeclarativeUI : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QGraphicsScene *scene READ scene WRITE setScene)
    Q_CLASSINFO("DefaultProperty", "scene")
public:
    QGraphicsViewDeclarativeUI(QObject *other) : QObject(other) {}

    QGraphicsScene *scene() const { return static_cast<QGraphicsView *>(parent())->scene(); }
    void setScene(QGraphicsScene *scene)
    {
        static_cast<QGraphicsView *>(parent())->setScene(scene);
    }
};

class QGraphicsSceneDeclarativeUI : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QmlList<QObject *> *children READ children)
    Q_CLASSINFO("DefaultProperty", "children")
public:
    QGraphicsSceneDeclarativeUI(QObject *other) : QObject(other), _children(other) {}

    QmlList<QObject *> *children() { return &_children; }

private:
    class Children : public QmlConcreteList<QObject *>
    {
    public:
        Children(QObject *scene) : q(scene) {}
        virtual void append(QObject *o)
        {
            insert(-1, o);
        }
        virtual void clear()
        {
            for (int i = 0; i < count(); ++i)
                if (QGraphicsObject *go = qobject_cast<QGraphicsObject *>(at(i)))
                    static_cast<QGraphicsScene *>(q)->removeItem(go);
            QmlConcreteList<QObject *>::clear();
        }
        virtual void removeAt(int i)
        {
            if (QGraphicsObject *go = qobject_cast<QGraphicsObject *>(at(i)))
                static_cast<QGraphicsScene *>(q)->removeItem(go);
            QmlConcreteList<QObject *>::removeAt(i);
        }
        virtual void insert(int i, QObject *o)
        {
            QmlConcreteList<QObject *>::insert(i, o);
            if (QGraphicsObject *go = qobject_cast<QGraphicsObject *>(o))
                static_cast<QGraphicsScene *>(q)->addItem(go);
            //else if (QWidget *w = qobject_cast<QWidget *>(o))
            //    static_cast<QGraphicsScene *>(q)->addWidget(w);
        }
    private:
        QObject *q;
    };
    Children _children;
};

class QGraphicsWidgetDeclarativeUI : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QmlList<QObject *> *data READ data)
    Q_PROPERTY(QmlList<QGraphicsItem *> *children READ children)
    Q_PROPERTY(QGraphicsLayout *layout READ layout WRITE setLayout)
    Q_CLASSINFO("DefaultProperty", "children")
public:
    QGraphicsWidgetDeclarativeUI(QObject *other) : QObject(other), _widgets(this) {}

    QmlList<QObject *> *data() { return &_data; }

    QmlList<QGraphicsItem *> *children() { return &_widgets; }

    QGraphicsLayout *layout() const { return static_cast<QGraphicsWidget *>(parent())->layout(); }
    void setLayout(QGraphicsLayout *lo)
    {
        static_cast<QGraphicsWidget *>(parent())->setLayout(lo);
    }

private:
    friend class WidgetList;
    void setItemParent(QGraphicsItem *wid)
    {
        wid->setParentItem(static_cast<QGraphicsWidget *>(parent()));
    }

    class WidgetList : public QmlConcreteList<QGraphicsItem *>
    {
    public:
        WidgetList(QGraphicsWidgetDeclarativeUI *o)
            : obj(o) {}

        virtual void append(QGraphicsItem *w)  { QmlConcreteList<QGraphicsItem *>::append(w); obj->setItemParent(w); }
        virtual void clear() { QmlConcreteList<QGraphicsItem *>::clear(); } //###
        virtual void removeAt(int i) { QmlConcreteList<QGraphicsItem *>::removeAt(i); }     //###
        virtual void insert(int i, QGraphicsItem *item) { QmlConcreteList<QGraphicsItem *>::insert(i, item); obj->setItemParent(item); }

    private:
        QGraphicsWidgetDeclarativeUI *obj;
    };
    WidgetList _widgets;
    QmlConcreteList<QObject *> _data;
};

QML_DEFINE_EXTENDED_TYPE(Qt,4,6,QGraphicsView,QGraphicsView,QGraphicsViewDeclarativeUI)
QML_DEFINE_EXTENDED_TYPE(Qt,4,6,QGraphicsScene,QGraphicsScene,QGraphicsSceneDeclarativeUI)
QML_DEFINE_EXTENDED_TYPE(Qt,4,6,QGraphicsWidget,QGraphicsWidget,QGraphicsWidgetDeclarativeUI)

QML_DEFINE_INTERFACE(QGraphicsItem)

QT_END_NAMESPACE

#include <graphicswidgets.moc>
