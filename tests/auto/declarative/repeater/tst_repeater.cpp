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
#include <QtTest/QtTest>
#include <qlistmodelinterface.h>
#include <qmlview.h>
#include <qfxrepeater.h>
#include <qfxtext.h>
#include <qmlcontext.h>

class tst_QFxRepeater : public QObject
{
    Q_OBJECT
public:
    tst_QFxRepeater();

private slots:
    void stringList();

private:
    QmlView *createView(const QString &filename);
    template<typename T>
    T *findItem(QObject *parent, const QString &id);
};

tst_QFxRepeater::tst_QFxRepeater()
{
}

/*
The Repeater element creates children at its own position in its parent's
stacking order.  In this test we insert a repeater between two other Text
elements to test this.
*/
void tst_QFxRepeater::stringList()
{
    QmlView *canvas = createView(SRCDIR "/data/repeater.qml");

    QStringList data;
    data << "One";
    data << "Two";
    data << "Three";
    data << "Four";

    QmlContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testData", data);

    canvas->execute();
    qApp->processEvents();

    QFxRepeater *repeater = findItem<QFxRepeater>(canvas->root(), "repeater");
    QVERIFY(repeater != 0);

    QFxItem *container = findItem<QFxItem>(canvas->root(), "container");
    QVERIFY(container != 0);

    QCOMPARE(container->childItems().count(), data.count() + 3);

    bool saw_repeater = false;
    for (int i = 0; i < container->childItems().count(); ++i) {

        if (i == 0) {
            QFxText *name = qobject_cast<QFxText*>(container->childItems().at(i));
            QVERIFY(name != 0);
            QCOMPARE(name->text(), QLatin1String("Zero"));
        } else if (i == container->childItems().count() - 2) {
            // The repeater itself
            QFxRepeater *rep = qobject_cast<QFxRepeater*>(container->childItems().at(i));
            QCOMPARE(rep, repeater);
            saw_repeater = true;
            continue;
        } else if (i == container->childItems().count() - 1) {
            QFxText *name = qobject_cast<QFxText*>(container->childItems().at(i));
            QVERIFY(name != 0);
            QCOMPARE(name->text(), QLatin1String("Last"));
        } else {
            QFxText *name = qobject_cast<QFxText*>(container->childItems().at(i));
            QVERIFY(name != 0);
            QCOMPARE(name->text(), data.at(i-1));
        }
    }
    QVERIFY(saw_repeater);

    delete canvas;
}


QmlView *tst_QFxRepeater::createView(const QString &filename)
{
    QmlView *canvas = new QmlView(0);
    canvas->setFixedSize(240,320);

    QFile file(filename);
    file.open(QFile::ReadOnly);
    QString qml = file.readAll();
    canvas->setQml(qml, filename);

    return canvas;
}

template<typename T>
T *tst_QFxRepeater::findItem(QObject *parent, const QString &objectName)
{
    const QMetaObject &mo = T::staticMetaObject;
    if (mo.cast(parent) && (objectName.isEmpty() || parent->objectName() == objectName))
        return static_cast<T*>(parent);
    for (int i = 0; i < parent->children().count(); ++i) {
        QFxItem *item = findItem<T>(parent->children().at(i), objectName);
        if (item)
            return static_cast<T*>(item);
    }

    return 0;
}

QTEST_MAIN(tst_QFxRepeater)

#include "tst_repeater.moc"
