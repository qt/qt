/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QtDeclarative/qml.h>
#include <private/qmllistaccessor_p.h>
#include <QDebug>

class tst_QmlListAccessor : public QObject
{
    Q_OBJECT
public:
    tst_QmlListAccessor() {}

private slots:
    void invalid();
    void qmllist();
    void qlist();
    void qstringlist();
    void qvariantlist();
    void qobject();
    void instance();
    void integer();
};

void tst_QmlListAccessor::invalid()
{
    QmlListAccessor accessor;

    QCOMPARE(accessor.list(), QVariant());
    QVERIFY(!accessor.isValid());
    QCOMPARE(accessor.type(), QmlListAccessor::Invalid);

    QCOMPARE(accessor.count(), 0);
    QVERIFY(!accessor.append(QVariant(10)));
    QVERIFY(!accessor.insert(0, QVariant(10)));
    QVERIFY(!accessor.removeAt(0));
    QVERIFY(!accessor.clear());

    accessor.setList(QVariant());

    QCOMPARE(accessor.list(), QVariant());
    QVERIFY(!accessor.isValid());
    QCOMPARE(accessor.type(), QmlListAccessor::Invalid);

    QCOMPARE(accessor.count(), 0);
    QVERIFY(!accessor.append(QVariant(10)));
    QVERIFY(!accessor.insert(0, QVariant(10)));
    QVERIFY(!accessor.removeAt(0));
    QVERIFY(!accessor.clear());
}

void tst_QmlListAccessor::qmllist()
{
    QmlConcreteList<QObject*> list;
    QObject *obj = new QObject(this);
    QObject *obj2 = new QObject(this);
    QObject *obj3 = new QObject(this);

    list.append(obj);
    QVERIFY(list.count() == 1);
    QCOMPARE(list.at(0), obj);

    QmlListAccessor accessor;
    accessor.setList(qVariantFromValue((QmlList<QObject*>*)&list));
    QCOMPARE(accessor.list(), qVariantFromValue((QmlList<QObject*>*)&list));

    // type
    QCOMPARE(accessor.type(), QmlListAccessor::QmlList);

    // isValid
    QVERIFY(accessor.isValid());

    // count
    QCOMPARE(accessor.count(), 1);

    // at
    QCOMPARE(qvariant_cast<QObject*>(accessor.at(0)), obj);

    // append
    accessor.append(qVariantFromValue(obj2));
    QVERIFY(accessor.isValid());
    QCOMPARE(accessor.count(), 2);
    QCOMPARE(qvariant_cast<QObject*>(accessor.at(1)), obj2);
    QCOMPARE(list.count(), 2);
    QCOMPARE(list.at(0), obj);
    QCOMPARE(list.at(1), obj2);

    // insert
    accessor.insert(1, qVariantFromValue(obj3));
    QVERIFY(accessor.isValid());
    QCOMPARE(accessor.count(), 3);
    QCOMPARE(qvariant_cast<QObject*>(accessor.at(1)), obj3);
    QCOMPARE(list.count(), 3);
    QCOMPARE(list.at(0), obj);
    QCOMPARE(list.at(1), obj3);
    QCOMPARE(list.at(2), obj2);

    // removeAt
    accessor.removeAt(1);
    QVERIFY(accessor.isValid());
    QCOMPARE(accessor.count(), 2);
    QCOMPARE(qvariant_cast<QObject*>(accessor.at(1)), obj2);
    QCOMPARE(list.count(), 2);
    QCOMPARE(list.at(0), obj);
    QCOMPARE(list.at(1), obj2);

    // clear
    accessor.clear();
    QVERIFY(accessor.isValid());
    QCOMPARE(accessor.count(), 0);
    QCOMPARE(list.count(), 0);
}

void tst_QmlListAccessor::qlist()
{
    QList<QObject*> list;
    QObject *obj = new QObject(this);
    QObject *obj2 = new QObject(this);
    QObject *obj3 = new QObject(this);

    list.append(obj);
    QVERIFY(list.count() == 1);
    QCOMPARE(list.at(0), obj);

    QmlListAccessor accessor;
    accessor.setList(qVariantFromValue((QList<QObject*>*)&list));
    QCOMPARE(accessor.list(), qVariantFromValue((QList<QObject*>*)&list));

    // type
    QCOMPARE(accessor.type(), QmlListAccessor::QListPtr);

    // isValid
    QVERIFY(accessor.isValid());

    // count
    QCOMPARE(accessor.count(), 1);

    // at
    QCOMPARE(qvariant_cast<QObject*>(accessor.at(0)), obj);

    // append
    accessor.append(qVariantFromValue(obj2));
    QVERIFY(accessor.isValid());
    QCOMPARE(accessor.count(), 2);
    QCOMPARE(qvariant_cast<QObject*>(accessor.at(1)), obj2);
    QCOMPARE(list.count(), 2);
    QCOMPARE(list.at(0), obj);
    QCOMPARE(list.at(1), obj2);

    // insert
    accessor.insert(1, qVariantFromValue(obj3));
    QVERIFY(accessor.isValid());
    QCOMPARE(accessor.count(), 3);
    QCOMPARE(qvariant_cast<QObject*>(accessor.at(1)), obj3);
    QCOMPARE(list.count(), 3);
    QCOMPARE(list.at(0), obj);
    QCOMPARE(list.at(1), obj3);
    QCOMPARE(list.at(2), obj2);

    // removeAt
    accessor.removeAt(1);
    QVERIFY(accessor.isValid());
    QCOMPARE(accessor.count(), 2);
    QCOMPARE(qvariant_cast<QObject*>(accessor.at(1)), obj2);
    QCOMPARE(list.count(), 2);
    QCOMPARE(list.at(0), obj);
    QCOMPARE(list.at(1), obj2);

    // clear
    accessor.clear();
    QVERIFY(accessor.isValid());
    QCOMPARE(accessor.count(), 0);
    QCOMPARE(list.count(), 0);
}

void tst_QmlListAccessor::qstringlist()
{
    QStringList list;
    list.append(QLatin1String("Item1"));
    list.append(QLatin1String("Item2"));
    QVERIFY(list.count() == 2);

    QmlListAccessor accessor;
    accessor.setList(list);

    // type
    QCOMPARE(accessor.type(), QmlListAccessor::StringList);

    // isValid
    QVERIFY(accessor.isValid());

    // count
    QVERIFY(accessor.count() == 2);

    // at
    QCOMPARE(qvariant_cast<QString>(accessor.at(0)), QLatin1String("Item1"));
    QCOMPARE(qvariant_cast<QString>(accessor.at(1)), QLatin1String("Item2"));

    // append
    QVERIFY(!accessor.append(QVariant("Item3")));
    QVERIFY(accessor.isValid());
    QVERIFY(accessor.count() == 2);
    QCOMPARE(qvariant_cast<QString>(accessor.at(0)), QLatin1String("Item1"));
    QCOMPARE(qvariant_cast<QString>(accessor.at(1)), QLatin1String("Item2"));

    // insert
    QVERIFY(!accessor.insert(1, QVariant("MiddleItem")));
    QVERIFY(accessor.isValid());
    QVERIFY(accessor.count() == 2);
    QCOMPARE(qvariant_cast<QString>(accessor.at(0)), QLatin1String("Item1"));
    QCOMPARE(qvariant_cast<QString>(accessor.at(1)), QLatin1String("Item2"));

    // removeAt
    QVERIFY(!accessor.removeAt(1));
    QVERIFY(accessor.isValid());
    QVERIFY(accessor.count() == 2);
    QCOMPARE(qvariant_cast<QString>(accessor.at(0)), QLatin1String("Item1"));
    QCOMPARE(qvariant_cast<QString>(accessor.at(1)), QLatin1String("Item2"));

    // clear
    QVERIFY(!accessor.clear());
    QVERIFY(accessor.isValid());
    QVERIFY(accessor.count() == 2);
    QCOMPARE(qvariant_cast<QString>(accessor.at(0)), QLatin1String("Item1"));
    QCOMPARE(qvariant_cast<QString>(accessor.at(1)), QLatin1String("Item2"));
}

void tst_QmlListAccessor::qvariantlist()
{
    QVariantList list;
    list.append(QLatin1String("Item1"));
    list.append(QLatin1String("Item2"));
    QVERIFY(list.count() == 2);

    QmlListAccessor accessor;
    accessor.setList(list);

    // type
    QCOMPARE(accessor.type(), QmlListAccessor::VariantList);

    // isValid
    QVERIFY(accessor.isValid());

    // count
    QVERIFY(accessor.count() == 2);

    // at
    QCOMPARE(qvariant_cast<QString>(accessor.at(0)), QLatin1String("Item1"));
    QCOMPARE(qvariant_cast<QString>(accessor.at(1)), QLatin1String("Item2"));

    // append
    QVERIFY(!accessor.append(QVariant("Item3")));
    QVERIFY(accessor.isValid());
    QVERIFY(accessor.count() == 2);
    QCOMPARE(qvariant_cast<QString>(accessor.at(0)), QLatin1String("Item1"));
    QCOMPARE(qvariant_cast<QString>(accessor.at(1)), QLatin1String("Item2"));

    // insert
    QVERIFY(!accessor.insert(1, QVariant("MiddleItem")));
    QVERIFY(accessor.isValid());
    QVERIFY(accessor.count() == 2);
    QCOMPARE(qvariant_cast<QString>(accessor.at(0)), QLatin1String("Item1"));
    QCOMPARE(qvariant_cast<QString>(accessor.at(1)), QLatin1String("Item2"));

    // removeAt
    QVERIFY(!accessor.removeAt(1));
    QVERIFY(accessor.isValid());
    QVERIFY(accessor.count() == 2);
    QCOMPARE(qvariant_cast<QString>(accessor.at(0)), QLatin1String("Item1"));
    QCOMPARE(qvariant_cast<QString>(accessor.at(1)), QLatin1String("Item2"));

    // clear
    QVERIFY(!accessor.clear());
    QVERIFY(accessor.isValid());
    QVERIFY(accessor.count() == 2);
    QCOMPARE(qvariant_cast<QString>(accessor.at(0)), QLatin1String("Item1"));
    QCOMPARE(qvariant_cast<QString>(accessor.at(1)), QLatin1String("Item2"));
}

void tst_QmlListAccessor::qobject()
{
    QObject *obj = new QObject(this);

    QmlListAccessor accessor;
    accessor.setList(qVariantFromValue(obj));

    // type
    QCOMPARE(accessor.type(), QmlListAccessor::Instance);

    // isValid
    QVERIFY(accessor.isValid());

    // count
    QVERIFY(accessor.count() == 1);

    // at
    QCOMPARE(accessor.at(0), qVariantFromValue(obj));

    // append
    QVERIFY(!accessor.append(qVariantFromValue((QObject *)0)));
    QVERIFY(accessor.isValid());
    QVERIFY(accessor.count() == 1);
    QCOMPARE(accessor.at(0), qVariantFromValue(obj));

    // insert
    QVERIFY(!accessor.insert(0, qVariantFromValue((QObject *)0)));
    QVERIFY(accessor.isValid());
    QVERIFY(accessor.count() == 1);
    QCOMPARE(accessor.at(0), qVariantFromValue(obj));

    // removeAt
    QVERIFY(!accessor.removeAt(0));
    QVERIFY(accessor.isValid());
    QVERIFY(accessor.count() == 1);
    QCOMPARE(accessor.at(0), qVariantFromValue(obj));

    // clear
    QVERIFY(!accessor.clear());
    QVERIFY(accessor.isValid());
    QVERIFY(accessor.count() == 1);
    QCOMPARE(accessor.at(0), qVariantFromValue(obj));
}

void tst_QmlListAccessor::instance()
{
    QRect r;

    QmlListAccessor accessor;
    accessor.setList(r);

    // type
    QCOMPARE(accessor.type(), QmlListAccessor::Instance);

    // isValid
    QVERIFY(accessor.isValid());

    // count
    QVERIFY(accessor.count() == 1);

    // at
    QCOMPARE(accessor.at(0), qVariantFromValue(r));

    // append
    QVERIFY(!accessor.append(qVariantFromValue(r)));
    QVERIFY(accessor.isValid());
    QVERIFY(accessor.count() == 1);
    QCOMPARE(accessor.at(0), qVariantFromValue(r));

    // insert
    QVERIFY(!accessor.insert(0, qVariantFromValue(r)));
    QVERIFY(accessor.isValid());
    QVERIFY(accessor.count() == 1);
    QCOMPARE(accessor.at(0), qVariantFromValue(r));

    // removeAt
    QVERIFY(!accessor.removeAt(0));
    QVERIFY(accessor.isValid());
    QVERIFY(accessor.count() == 1);
    QCOMPARE(accessor.at(0), qVariantFromValue(r));

    // clear
    QVERIFY(!accessor.clear());
    QVERIFY(accessor.isValid());
    QVERIFY(accessor.count() == 1);
    QCOMPARE(accessor.at(0), qVariantFromValue(r));
}

void tst_QmlListAccessor::integer()
{
    int r = 13;

    QmlListAccessor accessor;
    accessor.setList(r);

    // type
    QCOMPARE(accessor.type(), QmlListAccessor::Integer);

    // isValid
    QVERIFY(accessor.isValid());

    // count
    QVERIFY(accessor.count() == 13);

    // at
    QCOMPARE(accessor.at(4), qVariantFromValue(4));

    // append
    QVERIFY(!accessor.append(qVariantFromValue(r)));
    QVERIFY(accessor.isValid());
    QVERIFY(accessor.count() == 13);
    QCOMPARE(accessor.at(4), qVariantFromValue(4));

    // insert
    QVERIFY(!accessor.insert(0, qVariantFromValue(r)));
    QVERIFY(accessor.isValid());
    QVERIFY(accessor.count() == 13);
    QCOMPARE(accessor.at(4), qVariantFromValue(4));

    // removeAt
    QVERIFY(!accessor.removeAt(0));
    QVERIFY(accessor.isValid());
    QVERIFY(accessor.count() == 13);
    QCOMPARE(accessor.at(4), qVariantFromValue(4));

    // clear
    QVERIFY(!accessor.clear());
    QVERIFY(accessor.isValid());
    QVERIFY(accessor.count() == 13);
    QCOMPARE(accessor.at(4), qVariantFromValue(4));
}

QTEST_MAIN(tst_QmlListAccessor)

#include "tst_qmllistaccessor.moc"
