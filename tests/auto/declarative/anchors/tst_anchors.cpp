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
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qmlview.h>
#include <QtDeclarative/qfxrect.h>
#include <QtDeclarative/private/qfxanchors_p.h>


class tst_anchors : public QObject
{
    Q_OBJECT
public:
    tst_anchors() {}

    template<typename T>
    T *findItem(QFxItem *parent, const QString &id);

private slots:
    void basicAnchors();
    void loops();
    void illegalSets();
    void reset();
    void nullItem();
};

/*
   Find an item with the specified id.
*/
template<typename T>
T *tst_anchors::findItem(QFxItem *parent, const QString &objectName)
{
    const QMetaObject &mo = T::staticMetaObject;
    QList<QGraphicsItem *> children = parent->childItems();
    for (int i = 0; i < children.count(); ++i) {
        QFxItem *item = qobject_cast<QFxItem *>(children.at(i)->toGraphicsObject());
        if (item) {
            if (mo.cast(item) && (objectName.isEmpty() || item->objectName() == objectName)) {
                return static_cast<T*>(item);
            }
            item = findItem<T>(item, objectName);
            if (item)
                return static_cast<T*>(item);
        }
    }

    return 0;
}

void tst_anchors::basicAnchors()
{
    QmlView *view = new QmlView;
    view->setUrl(QUrl("file://" SRCDIR "/data/anchors.qml"));

    view->execute();
    qApp->processEvents();

    //sibling horizontal
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect1"))->x(), 26.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect2"))->x(), 122.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect3"))->x(), 74.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect4"))->x(), 16.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect5"))->x(), 112.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect6"))->x(), 64.0);

    //parent horizontal
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect7"))->x(), 0.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect8"))->x(), 240.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect9"))->x(), 120.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect10"))->x(), -10.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect11"))->x(), 230.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect12"))->x(), 110.0);

    //vertical
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect13"))->y(), 20.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect14"))->y(), 155.0);

    //stretch
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect15"))->x(), 26.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect15"))->width(), 96.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect16"))->x(), 26.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect16"))->width(), 192.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect17"))->x(), -70.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect17"))->width(), 192.0);

    //vertical stretch
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect18"))->y(), 20.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect18"))->height(), 40.0);

    //more parent horizontal
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect19"))->x(), 115.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect20"))->x(), 235.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect21"))->x(), -5.0);

    delete view;
}

// mostly testing that we don't crash
void tst_anchors::loops()
{
    {
        QmlView *view = new QmlView;

        view->setUrl(QUrl("file://" SRCDIR "/data/loop1.qml"));

        QString expect = "QML QFxText (" + view->url().toString() + ":7:5" + ") Possible anchor loop detected on horizontal anchor.";
        QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());
        QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());
        QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());
        view->execute();
        qApp->processEvents();

        delete view;
    }

    {
        QmlView *view = new QmlView;

        view->setUrl(QUrl("file://" SRCDIR "/data/loop2.qml"));

        QString expect = "QML QFxImage (" + view->url().toString() + ":14:3" + ") Possible anchor loop detected on horizontal anchor.";
        QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());
        view->execute();
        qApp->processEvents();

        delete view;
    }
}

void tst_anchors::illegalSets()
{
    {
        QmlView *view = new QmlView;

        view->setUrl(QUrl("file://" SRCDIR "/data/illegal1.qml"));

        QString expect = "QML QFxRect (" + view->url().toString() + ":7:5" + ") Can't specify left, right, and hcenter anchors.";
        QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());
        view->execute();
        qApp->processEvents();

        delete view;
    }

    {
        QmlView *view = new QmlView;

        view->setUrl(QUrl("file://" SRCDIR "/data/illegal2.qml"));

        QString expect = "QML QFxText (" + view->url().toString() + ":7:5" + ") Baseline anchor can't be used in conjunction with top, bottom, or vcenter anchors.";
        QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());
        view->execute();
        //qApp->processEvents();

        delete view;
    }

    {
        QmlView *view = new QmlView;

        view->setUrl(QUrl("file://" SRCDIR "/data/illegal3.qml"));

        QString expect = "QML QFxRect (" + view->url().toString() + ":9:5" + ") Can't anchor to an item that isn't a parent or sibling.";
        QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());
        view->execute();
        //qApp->processEvents();

        delete view;
    }
}

void tst_anchors::reset()
{
    QFxItem *aItem = new QFxItem;
    QFxAnchorLine anchor;
    anchor.item = aItem;
    anchor.anchorLine = QFxAnchorLine::Top;

    QFxItem *item = new QFxItem;
    item->anchors()->setBottom(anchor);
    QCOMPARE(item->anchors()->usedAnchors().testFlag(QFxAnchors::HasBottomAnchor), true);

    item->anchors()->resetBottom();
    QCOMPARE(item->anchors()->usedAnchors().testFlag(QFxAnchors::HasBottomAnchor), false);
}

void tst_anchors::nullItem()
{
    QFxAnchorLine anchor;

    QTest::ignoreMessage(QtWarningMsg, "QML QFxItem (unknown location) Can't anchor to a null item.");
    QFxItem *item = new QFxItem;
    item->anchors()->setBottom(anchor);
}

QTEST_MAIN(tst_anchors)

#include "tst_anchors.moc"
