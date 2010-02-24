/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include <QtDeclarative/qdeclarativeextensionplugin.h>
#include <QtDeclarative/qdeclarative.h>

#include "graphicslayouts_p.h"
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

    Q_PROPERTY(QDeclarativeListProperty<QObject> children READ children)
    Q_CLASSINFO("DefaultProperty", "children")
public:
    QGraphicsSceneDeclarativeUI(QObject *other) : QObject(other) {}

    QDeclarativeListProperty<QObject> children() { return QDeclarativeListProperty<QObject>(this->parent(), 0, children_append); }

private:
    static void children_append(QDeclarativeListProperty<QObject> *prop, QObject *o) {
        if (QGraphicsObject *go = qobject_cast<QGraphicsObject *>(o))
            static_cast<QGraphicsScene *>(prop->object)->addItem(go);
    }
};

class QGraphicsWidgetDeclarativeUI : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeListProperty<QGraphicsItem> children READ children)
    Q_PROPERTY(QGraphicsLayout *layout READ layout WRITE setLayout)
    Q_CLASSINFO("DefaultProperty", "children")
public:
    QGraphicsWidgetDeclarativeUI(QObject *other) : QObject(other) {}

    QDeclarativeListProperty<QGraphicsItem> children() { return QDeclarativeListProperty<QGraphicsItem>(this, 0, children_append); }

    QGraphicsLayout *layout() const { return static_cast<QGraphicsWidget *>(parent())->layout(); }
    void setLayout(QGraphicsLayout *lo)
    {
        static_cast<QGraphicsWidget *>(parent())->setLayout(lo);
    }

private:
    void setItemParent(QGraphicsItem *wid)
    {
        wid->setParentItem(static_cast<QGraphicsWidget *>(parent()));
    }

    static void children_append(QDeclarativeListProperty<QGraphicsItem> *prop, QGraphicsItem *i) {
        static_cast<QGraphicsWidgetDeclarativeUI*>(prop->object)->setItemParent(i);
    }
};

class QWidgetsQmlModule : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    QStringList keys() const
    {
        return QStringList() << QLatin1String("Qt.widgets");
    }

    virtual void initialize(QDeclarativeEngine *engine, const char *uri)
    {
        Q_UNUSED(engine);

        Q_ASSERT(QLatin1String(uri) == QLatin1String("Qt.widgets"));

        QML_REGISTER_INTERFACE(QGraphicsLayoutItem);
        QML_REGISTER_INTERFACE(QGraphicsLayout);
        qmlRegisterType<QGraphicsLinearLayoutStretchItemObject>(uri,4,6,"QGraphicsLinearLayoutStretchItem","QGraphicsLinearLayoutStretchItemObject");
        qmlRegisterType<QGraphicsLinearLayoutObject>(uri,4,6,"QGraphicsLinearLayout","QGraphicsLinearLayoutObject");
        qmlRegisterType<QGraphicsGridLayoutObject>(uri,4,6,"QGraphicsGridLayout","QGraphicsGridLayoutObject");
        qmlRegisterExtendedType<QGraphicsView, QGraphicsViewDeclarativeUI>(uri,4,6,"QGraphicsView","QGraphicsView");
        qmlRegisterExtendedType<QGraphicsScene,QGraphicsSceneDeclarativeUI>(uri,4,6,"QGraphicsScene","QGraphicsScene");
        qmlRegisterExtendedType<QGraphicsWidget,QGraphicsWidgetDeclarativeUI>(uri,4,6,"QGraphicsWidget","QGraphicsWidget");
        QML_REGISTER_INTERFACE(QGraphicsItem);
    }
};

QT_END_NAMESPACE

#include "widgets.moc"

Q_EXPORT_PLUGIN2(qtwidgetsqmlmodule, QT_PREPEND_NAMESPACE(QWidgetsQmlModule));

