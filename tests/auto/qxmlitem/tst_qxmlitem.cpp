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


#include <QtTest/QtTest>

#ifdef QTEST_XMLPATTERNS

#include <QXmlItem>

/*!
 \class tst_QXmlItem
 \internal
 \since 4.4
 \brief Tests class QXmlItem.
 */
class tst_QXmlItem : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void defaultConstructor() const;
    void copyConstructor() const;
    void copyConstructorFromQVariant() const;
    void copyConstructorFromQXmlNodeModelIndex() const;
    void assignmentOperator() const;
    void isNull() const;
    void isNode() const;
    void isAtomicValue() const;
    void toAtomicValue() const;
    void toNodeModelIndex() const;

    void objectSize() const;
    void constCorrectness() const;
    void withinQVariant() const;
};

void tst_QXmlItem::defaultConstructor() const
{
    {
        QXmlItem();
    }

    {
        QXmlItem();
        QXmlItem();
    }

    {
        QXmlItem();
        QXmlItem();
        QXmlItem();
    }
}

void tst_QXmlItem::copyConstructor() const
{
    /* Check that we can copy from a const reference. */
    {
        const QXmlItem item;
        const QXmlItem copy(item);
    }

    /* On a QXmlItem constructed from a null QVariant. */
    {
        const QXmlItem item((QVariant()));
        const QXmlItem copy(item);
    }

    /* On a QXmlItem constructed from a null QXmlNodeModelIndex. */
    {
        const QXmlItem item((QXmlNodeModelIndex()));
        const QXmlItem copy(item);
    }
}

void tst_QXmlItem::copyConstructorFromQVariant() const
{
    /* Construct & destruct a single value. */
    {
        const QXmlItem item(QVariant(QString::fromLatin1("dummy")));
    }

    /* Copy a null QVariant. */
    {
        const QXmlItem item((QVariant()));
        QVERIFY(item.isNull());
    }

}

void tst_QXmlItem::copyConstructorFromQXmlNodeModelIndex() const
{
    // TODO copy a valid model index.

    /* Construct from a null QXmlNodeModelIndex. */
    {
        const QXmlItem item((QXmlNodeModelIndex()));
        QVERIFY(item.isNull());
    }
}

void tst_QXmlItem::assignmentOperator() const
{
    /* Assign to self. */
    {
        /* With null value. */
        {
            QXmlItem item;
            item = item;
            item = item;
            item = item;
            item = item;
            item = item;
        }

        /* With the same atomic value. */
        {
            QXmlItem item(QVariant(QString::fromLatin1("dummy")));
            item = item;
            item = item;
            item = item;
            item = item;
            item = item;
        }

        /* With the same node. */
        {
            // TODO
        }

        /* With a QXmlItem constructed from a null QVariant. */
        {
            QXmlItem item((QVariant()));
            item = item;
            item = item;
            item = item;
            item = item;
            item = item;
        }

        /* With a QXmlItem constructed from a null QXmlNodeModelIndex. */
        {
            QXmlItem item((QXmlNodeModelIndex()));
            item = item;
            item = item;
            item = item;
            item = item;
            item = item;
        }
    }
}

void tst_QXmlItem::isNull() const
{
    /* Check default value. */
    {
        const QXmlItem item;
        QVERIFY(item.isNull());
    }

    /* On atomic value. */
    {
        const QXmlItem item(QVariant(3));
        QVERIFY(!item.isNull());
    }

    /* On a QXmlItem constructed from a null QVariant. */
    {
        const QXmlItem item((QVariant()));
        QVERIFY(item.isNull());
    }

    /* On a QXmlItem constructed from a null QXmlNodeModelIndex. */
    {
        const QXmlItem item((QXmlNodeModelIndex()));
        QVERIFY(item.isNull());
    }
}

void tst_QXmlItem::isNode() const
{
    /* Check default value. */
    {
        const QXmlItem item;
        QVERIFY(!item.isNode());
    }

    /* On atomic value. */
    {
        const QXmlItem item(QVariant(3));
        QVERIFY(!item.isNode());
    }
    // TODO on valid node index

    /* On a QXmlItem constructed from a null QVariant. */
    {
        const QXmlItem item((QVariant()));
        QVERIFY(!item.isNode());
    }

    /* On a QXmlItem constructed from a null QXmlNodeModelIndex. */
    {
        const QXmlItem item((QXmlNodeModelIndex()));
        QVERIFY(!item.isNode());
    }
}

void tst_QXmlItem::isAtomicValue() const
{
    /* Check default value. */
    {
        const QXmlItem item;
        QVERIFY(!item.isAtomicValue());
    }

    /* On valid atomic value. */
    {
        const QXmlItem item(QVariant(3));
        QVERIFY(item.isAtomicValue());
    }

    // TODO on valid node index

    /* On a QXmlItem constructed from a null QVariant. */
    {
        const QXmlItem item((QVariant()));
        QVERIFY(!item.isAtomicValue());
    }

    /* On a QXmlItem constructed from a null QXmlNodeModelIndex. */
    {
        const QXmlItem item((QXmlNodeModelIndex()));
        QVERIFY(!item.isAtomicValue());
    }
}

void tst_QXmlItem::toAtomicValue() const
{
    /* Check default value. */
    {
        const QXmlItem item;
        QVERIFY(item.toAtomicValue().isNull());
    }

    /* On atomic value. */
    {
        const QXmlItem item(QVariant(3));
        QCOMPARE(item.toAtomicValue(), QVariant(3));
    }

    /* On a QXmlItem constructed from a null QVariant. */
    {
        const QXmlItem item((QVariant()));
        QVERIFY(item.toAtomicValue().isNull());
    }

    /* On a QXmlItem constructed from a null QXmlNodeModelIndex. */
    {
        const QXmlItem item((QXmlNodeModelIndex()));
        QVERIFY(item.toAtomicValue().isNull());
    }
}

void tst_QXmlItem::toNodeModelIndex() const
{
    /* Check default value. */
    {
        const QXmlItem item;
        QVERIFY(item.toNodeModelIndex().isNull());
    }

    /* On valid atomic value. */
    {
        const QXmlItem item(QVariant(3));
        QVERIFY(item.toNodeModelIndex().isNull());
    }

    /* On a QXmlItem constructed from a null QVariant. */
    {
        const QXmlItem item((QVariant()));
        QVERIFY(item.isNull());
    }

    /* On a QXmlItem constructed from a null QXmlNodeModelIndex. */
    {
        const QXmlItem item((QXmlNodeModelIndex()));
        QVERIFY(item.isNull());
    }
}

void tst_QXmlItem::objectSize() const
{
    /* We can't currently test this in portable way,
     * so disable it. */
    return;

    QCOMPARE(sizeof(QPatternist::NodeIndexStorage), sizeof(QXmlItem));

    /* Data, additional data, and pointer to model. We test for two, such that we
     * account for the padding that MSVC do. */
    QVERIFY(sizeof(QXmlItem) == sizeof(qint64) * 2 + sizeof(QAbstractXmlNodeModel *) * 2
            || sizeof(QXmlItem) == sizeof(qint64) * 2 + sizeof(QAbstractXmlNodeModel *) * 2);
}

/*!
  Check that the functions that should be const, are.
 */
void tst_QXmlItem::constCorrectness() const
{
    const QXmlItem item;
    item.isNull();
    item.isNode();
    item.isAtomicValue();

    item.toAtomicValue();
    item.toNodeModelIndex();
}

/*!
  Check that QXmlItem can be used inside QVariant.
 */
void tst_QXmlItem::withinQVariant() const
{
    QXmlItem val;
    const QVariant variant(qVariantFromValue(val));
    QXmlItem val2(qVariantValue<QXmlItem>(variant));
}

QTEST_MAIN(tst_QXmlItem)

#include "tst_qxmlitem.moc"
#else //QTEST_XMLPATTERNS
QTEST_NOOP_MAIN
#endif
