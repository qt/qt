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
#include <QtDeclarative/qml.h>
#include <QtDeclarative/qmllistaccessor.h>

class tst_QmlListAccessor : public QObject
{
    Q_OBJECT
public:
    tst_QmlListAccessor() {}

private slots:
    void qmllist();
    //void qlist();
    //void qstringlist();
};

void tst_QmlListAccessor::qmllist()
{
    QmlConcreteList<QObject*> list;
    QObject *obj = new QObject;
    list.append(obj);
    QVERIFY(list.count() == 1);
    QCOMPARE(list.at(0), obj);

    QmlListAccessor accessor;
    accessor.setList(qVariantFromValue((QmlList<QObject*>*)&list));

    QVERIFY(accessor.isValid());
    QVERIFY(accessor.count() == 1);

    QVariant v = accessor.at(0);
    QCOMPARE(qvariant_cast<QObject*>(v), obj);

    accessor.removeAt(3);
    QVERIFY(accessor.count() == 1);

    accessor.removeAt(0);
    QVERIFY(accessor.count() == 0);

    accessor.insert(4, qVariantFromValue(obj));
    QVERIFY(accessor.count() == 1);

    v = accessor.at(0);
    QCOMPARE(qvariant_cast<QObject*>(v), obj);

    QObject *obj2 = new QObject;
    accessor.append(qVariantFromValue(obj2));
    QVERIFY(accessor.count() == 2);

    v = accessor.at(1);
    QCOMPARE(qvariant_cast<QObject*>(v), obj2);

    accessor.clear();
    QVERIFY(accessor.count() == 0);

    QVERIFY(accessor.isValid());
}

QTEST_MAIN(tst_QmlListAccessor)

#include "tst_qmllistaccessor.moc"
