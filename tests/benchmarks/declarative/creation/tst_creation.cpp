/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#include <qtest.h>
#include <QmlEngine>
#include <QmlComponent>
#include <QmlMetaType>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QmlGraphicsItem>
#include <private/qobject_p.h>

class tst_creation : public QObject
{
    Q_OBJECT
public:
    tst_creation() {}

private slots:
    void qobject_cpp();
    void qobject_qml();
    void qobject_qmltype();
    void qobject_alloc();

    void objects_qmltype_data();
    void objects_qmltype();

    void qgraphicsitem();
    void qgraphicsobject();
    void qgraphicsitem14();
    void qgraphicsitem_tree14();

    void itemtree_notree_cpp();
    void itemtree_objtree_cpp();
    void itemtree_cpp();
    void itemtree_data_cpp();
    void itemtree_qml();
    void itemtree_scene_cpp();

private:
    QmlEngine engine;
};

inline QUrl TEST_FILE(const QString &filename)
{
    return QUrl::fromLocalFile(QLatin1String(SRCDIR) + QLatin1String("/data/") + filename);
}

void tst_creation::qobject_cpp()
{
    QBENCHMARK {
        QObject *obj = new QObject;
        delete obj;
    }
}

void tst_creation::qobject_qml()
{
    QmlComponent component(&engine, TEST_FILE("qobject.qml"));
    QObject *obj = component.create();
    delete obj;

    QBENCHMARK {
        QObject *obj = component.create();
        delete obj;
    }
}

void tst_creation::qobject_qmltype()
{
    QmlType *t = QmlMetaType::qmlType("Qt/QtObject", 4, 6);

    QBENCHMARK {
        QObject *obj = t->create();
        delete obj;
    }
}

struct QObjectFakeData {
    char data[sizeof(QObjectPrivate)];
};

struct QObjectFake {
    QObjectFake();
    virtual ~QObjectFake();
private:
    QObjectFakeData *d;
};

QObjectFake::QObjectFake()
{
    d = new QObjectFakeData;
}

QObjectFake::~QObjectFake()
{
    delete d;
}

void tst_creation::qobject_alloc()
{
    QBENCHMARK {
        QObjectFake *obj = new QObjectFake;
        delete obj;
    }
}

void tst_creation::objects_qmltype_data()
{
    QTest::addColumn<QByteArray>("type");

    QList<QByteArray> types = QmlMetaType::qmlTypeNames();
    foreach (QByteArray type, types)
        QTest::newRow(type.constData()) << type;
}

void tst_creation::objects_qmltype()
{
    QFETCH(QByteArray, type);
    QmlType *t = QmlMetaType::qmlType(type, 4, 6);

    QBENCHMARK {
        QObject *obj = t->create();
        delete obj;
    }
}

class QGraphicsItemDummy : public QGraphicsItem
{
public:
    virtual QRectF boundingRect() const { return QRectF(); }
    virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) {}
};

class QGraphicsObjectDummy : public QGraphicsObject
{
public:
    virtual QRectF boundingRect() const { return QRectF(); }
    virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) {}
};

void tst_creation::qgraphicsitem()
{
    QBENCHMARK {
        QGraphicsItemDummy *i = new QGraphicsItemDummy();
        delete i;
    }
}

void tst_creation::qgraphicsobject()
{
    QBENCHMARK {
        QGraphicsObjectDummy *i = new QGraphicsObjectDummy();
        delete i;
    }
}

void tst_creation::qgraphicsitem14()
{
    QBENCHMARK {
        QGraphicsItemDummy *i1 = new QGraphicsItemDummy();
        QGraphicsItemDummy *i2 = new QGraphicsItemDummy();
        QGraphicsItemDummy *i3 = new QGraphicsItemDummy();
        QGraphicsItemDummy *i4 = new QGraphicsItemDummy();
        QGraphicsItemDummy *i5 = new QGraphicsItemDummy();
        QGraphicsItemDummy *i6 = new QGraphicsItemDummy();
        QGraphicsItemDummy *i7 = new QGraphicsItemDummy();
        QGraphicsItemDummy *i8 = new QGraphicsItemDummy();
        QGraphicsItemDummy *i9 = new QGraphicsItemDummy();
        QGraphicsItemDummy *i10 = new QGraphicsItemDummy();
        QGraphicsItemDummy *i11 = new QGraphicsItemDummy();
        QGraphicsItemDummy *i12 = new QGraphicsItemDummy();
        QGraphicsItemDummy *i13 = new QGraphicsItemDummy();
        QGraphicsItemDummy *i14 = new QGraphicsItemDummy();

        delete i1;
        delete i2;
        delete i3;
        delete i4;
        delete i5;
        delete i6;
        delete i7;
        delete i8;
        delete i9;
        delete i10;
        delete i11;
        delete i12;
        delete i13;
        delete i14;
    }
}

void tst_creation::qgraphicsitem_tree14()
{
    QBENCHMARK {
        // i1
        // +-------------------------+
        // i2                        i3
        // +-----------+       +-----+-----+
        // i4          i5      i6          i7
        // +----+   +--+    +--+--+        +----+
        // i8   i9  i10   i11     i12      i13  i14

        QGraphicsItemDummy *i1 = new QGraphicsItemDummy();
        QGraphicsItemDummy *i2 = new QGraphicsItemDummy();
        QGraphicsItemDummy *i3 = new QGraphicsItemDummy();
        QGraphicsItemDummy *i4 = new QGraphicsItemDummy();
        QGraphicsItemDummy *i5 = new QGraphicsItemDummy();
        QGraphicsItemDummy *i6 = new QGraphicsItemDummy();
        QGraphicsItemDummy *i7 = new QGraphicsItemDummy();
        QGraphicsItemDummy *i8 = new QGraphicsItemDummy();
        QGraphicsItemDummy *i9 = new QGraphicsItemDummy();
        QGraphicsItemDummy *i10 = new QGraphicsItemDummy();
        QGraphicsItemDummy *i11 = new QGraphicsItemDummy();
        QGraphicsItemDummy *i12 = new QGraphicsItemDummy();
        QGraphicsItemDummy *i13 = new QGraphicsItemDummy();
        QGraphicsItemDummy *i14 = new QGraphicsItemDummy();

        i14->setParentItem(i7);
        i13->setParentItem(i7);
        i12->setParentItem(i6);
        i11->setParentItem(i6);
        i10->setParentItem(i5);
        i9->setParentItem(i4);
        i8->setParentItem(i4);

        i7->setParentItem(i3);
        i6->setParentItem(i3);
        i5->setParentItem(i2);
        i4->setParentItem(i2);

        i3->setParentItem(i1);
        i2->setParentItem(i1);

        delete i1;
    }
}

struct QmlGraphics_DerivedObject : public QObject
{
    void setParent_noEvent(QObject *parent) {
        bool sce = d_ptr->sendChildEvents;
        d_ptr->sendChildEvents = false;
        setParent(parent);
        d_ptr->sendChildEvents = sce;
    }
};

inline void QmlGraphics_setParent_noEvent(QObject *object, QObject *parent)
{
    static_cast<QmlGraphics_DerivedObject *>(object)->setParent_noEvent(parent);
}

void tst_creation::itemtree_notree_cpp()
{
    QBENCHMARK {
        QmlGraphicsItem *item = new QmlGraphicsItem;
        for (int i = 0; i < 30; ++i) {
            QmlGraphicsItem *child = new QmlGraphicsItem;
        }
        delete item;
    }
}

void tst_creation::itemtree_objtree_cpp()
{
    QBENCHMARK {
        QmlGraphicsItem *item = new QmlGraphicsItem;
        for (int i = 0; i < 30; ++i) {
            QmlGraphicsItem *child = new QmlGraphicsItem;
            QmlGraphics_setParent_noEvent(child,item);
        }
        delete item;
    }
}

void tst_creation::itemtree_cpp()
{
    QBENCHMARK {
        QmlGraphicsItem *item = new QmlGraphicsItem;
        for (int i = 0; i < 30; ++i) {
            QmlGraphicsItem *child = new QmlGraphicsItem;
            QmlGraphics_setParent_noEvent(child,item);
            child->setParentItem(item);
        }
        delete item;
    }
}

void tst_creation::itemtree_data_cpp()
{
    QBENCHMARK {
        QmlGraphicsItem *item = new QmlGraphicsItem;
        for (int i = 0; i < 30; ++i) {
            QmlGraphicsItem *child = new QmlGraphicsItem;
            QmlGraphics_setParent_noEvent(child,item);
            item->data()->append(child);
        }
        delete item;
    }
}

void tst_creation::itemtree_qml()
{
    QmlComponent component(&engine, TEST_FILE("item.qml"));
    QObject *obj = component.create();
    delete obj;

    QBENCHMARK {
        QObject *obj = component.create();
        delete obj;
    }
}

void tst_creation::itemtree_scene_cpp()
{
    QGraphicsScene scene;
    QmlGraphicsItem *root = new QmlGraphicsItem;
    scene.addItem(root);
    QBENCHMARK {
        QmlGraphicsItem *item = new QmlGraphicsItem;
        for (int i = 0; i < 30; ++i) {
            QmlGraphicsItem *child = new QmlGraphicsItem;
            QmlGraphics_setParent_noEvent(child,item);
            child->setParentItem(item);
        }
        item->setParentItem(root);
        delete item;
    }
    delete root;
}


QTEST_MAIN(tst_creation)

#include "tst_creation.moc"
