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

#include "graphicswidgets.h"

QT_BEGIN_NAMESPACE

//### the single (default) property could alternatively be added to graphics view directly
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
QML_DEFINE_EXTENDED_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,QGraphicsView,QGraphicsView,QGraphicsViewDeclarativeUI)

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
                if (QGraphicsWidget *w = qobject_cast<QGraphicsWidget *>(at(i)))
                    static_cast<QGraphicsScene *>(q)->removeItem(w);
            QmlConcreteList<QObject *>::clear();
        }
        virtual void removeAt(int i)
        {
            if (QGraphicsWidget *w = qobject_cast<QGraphicsWidget *>(at(i)))
                static_cast<QGraphicsScene *>(q)->removeItem(w);
            QmlConcreteList<QObject *>::removeAt(i);
        }
        virtual void insert(int i, QObject *o)
        {
            QmlConcreteList<QObject *>::insert(i, o);

            //XXX are there any cases when insertion should be different from appension?
            if (QGraphicsWidget *w = qobject_cast<QGraphicsWidget *>(o))
                static_cast<QGraphicsScene *>(q)->addItem(w);
            //else if (QWidget *w = qobject_cast<QWidget *>(o))
            //    static_cast<QGraphicsScene *>(q)->addWidget(w);
            //else
            //    qWarning() << "Can't add" << o << "to a QGraphicsScene";
        }
    private:
        QObject *q;
    };
    Children _children;
};
QML_DEFINE_EXTENDED_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,QGraphicsScene,QGraphicsScene,QGraphicsSceneDeclarativeUI)

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

    //###
    void clearWidget()
    {
    }

    class WidgetList : public QmlConcreteList<QGraphicsItem *>
    {
    public:
        WidgetList(QGraphicsWidgetDeclarativeUI *o)
            : obj(o) {}

        virtual void append(QGraphicsItem *w)  { QmlConcreteList<QGraphicsItem *>::append(w); obj->setItemParent(w); }
        virtual void clear() { QmlConcreteList<QGraphicsItem *>::clear(); obj->clearWidget(); }
        virtual void removeAt(int i) { QmlConcreteList<QGraphicsItem *>::removeAt(i); }     //XXX
        virtual void insert(int i, QGraphicsItem *item) { QmlConcreteList<QGraphicsItem *>::insert(i, item); obj->setItemParent(item); }

    private:
        QGraphicsWidgetDeclarativeUI *obj;
    };
    WidgetList _widgets;
    QmlConcreteList<QObject *> _data;
};

QML_DEFINE_EXTENDED_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,QGraphicsWidget,QGraphicsWidget,QGraphicsWidgetDeclarativeUI)

QML_DEFINE_INTERFACE(QGraphicsItem)

QT_END_NAMESPACE

#include "graphicswidgets.moc"
