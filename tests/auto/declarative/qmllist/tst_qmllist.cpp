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
#include <QtDeclarative/qmlprivate.h>

class tst_QmlList : public QObject
{
    Q_OBJECT
public:
    tst_QmlList() {}

private slots:
    void interface();
};

void tst_QmlList::interface()
{
    QmlConcreteList<QObject*> list;
    QObject *obj = new QObject;
    obj->setObjectName("foo");
    list.append(obj);
    QVERIFY(list.count() == 1);
    QCOMPARE(list.at(0), obj);

    QmlPrivate::ListInterface *li = (QmlPrivate::ListInterface*)&list;

    void *ptr[1];
    li->at(0, ptr);
    QVERIFY(li->count() == 1);
    QCOMPARE(ptr[0], obj);

    li->removeAt(0);
    QVERIFY(li->count() == 0);
    QVERIFY(list.count() == 0);
}

QTEST_MAIN(tst_QmlList)

#include "tst_qmllist.moc"
