/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>

#ifdef QTEST_XMLPATTERNS

#include <QXmlNodeModelIndex>

/*!
 \class tst_QXmlNodeModelIndex
 \internal
 \since 4.4
 \brief Tests class QXmlNodeModelIndex.

 */
class tst_QXmlNodeModelIndex : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void copyConstructor() const;
    void constCorrectness() const;
    void assignmentOperator() const;
    void equalnessOperator() const;
    void inequalnessOperator() const;
    void objectSize() const;
    void internalPointer() const;
    void data() const;
    void additionalData() const;
    void isNull() const;
    void model() const;
    void withqHash() const;
};

void tst_QXmlNodeModelIndex::objectSize() const
{
    /* We can't do an exact comparison, because some platforms do padding. */
    QVERIFY(sizeof(QXmlNodeModelIndex) >= sizeof(QAbstractXmlNodeModel *) + sizeof(qint64) * 2);
}

void tst_QXmlNodeModelIndex::constCorrectness() const
{
    const QXmlNodeModelIndex index;
    /* All these functions should be const. */
    index.internalPointer();
    index.data();
    index.additionalData();
    index.isNull();
    index.model();
}

void tst_QXmlNodeModelIndex::assignmentOperator() const
{
    QXmlNodeModelIndex o1;
    const QXmlNodeModelIndex o2;
    o1 = o2;
    // TODO
}

void tst_QXmlNodeModelIndex::equalnessOperator() const
{
    QXmlNodeModelIndex o1;
    const QXmlNodeModelIndex o2;
    // TODO check const correctness
    o1 == o2;
}

void tst_QXmlNodeModelIndex::inequalnessOperator() const
{
    QXmlNodeModelIndex o1;
    const QXmlNodeModelIndex o2;
    // TODO check const correctness
    o1 != o2;
}

void tst_QXmlNodeModelIndex::copyConstructor() const
{
    /* Check that we can take a const reference. */
    {
        const QXmlNodeModelIndex index;
        const QXmlNodeModelIndex copy(index);
    }

    /* Take a copy of a temporary. */
    {
    	/* The extra paranthesis silences a warning on win32-msvc. */
        const QXmlNodeModelIndex copy((QXmlNodeModelIndex()));
    }
}

void tst_QXmlNodeModelIndex::internalPointer() const
{
    /* Check default value. */
    {
        const QXmlNodeModelIndex index;
        QCOMPARE(index.internalPointer(), static_cast<void *>(0));
    }
}

void tst_QXmlNodeModelIndex::data() const
{
    /* Check default value. */
    {
        const QXmlNodeModelIndex index;
        QCOMPARE(index.data(), qint64(0));
    }

    // TODO check that the return value for data() is qint64.
}

void tst_QXmlNodeModelIndex::additionalData() const
{
    /* Check default value. */
    {
        const QXmlNodeModelIndex index;
        QCOMPARE(index.additionalData(), qint64(0));
    }

    // TODO check that the return value for data() is qint64.
}

void tst_QXmlNodeModelIndex::isNull() const
{
    /* Check default value. */
    {
        const QXmlNodeModelIndex index;
        QVERIFY(index.isNull());
    }

    /* Test default value on a temporary object. */
    {
        QVERIFY(QXmlNodeModelIndex().isNull());
    }
}

void tst_QXmlNodeModelIndex::model() const
{
    /* Check default value. */
    {
        const QXmlNodeModelIndex index;
        QCOMPARE(index.model(), static_cast<const QAbstractXmlNodeModel *>(0));
    }
}

void tst_QXmlNodeModelIndex::withqHash() const
{
    QXmlNodeModelIndex null;
    qHash(null);
    //Do something which means operator== must be available.
}

QTEST_MAIN(tst_QXmlNodeModelIndex)

#include "tst_qxmlnodemodelindex.moc"
#else //QTEST_XMLPATTERNS
QTEST_NOOP_MAIN
#endif
