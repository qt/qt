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

#include "private/qabstractxmlforwarditerator_p.h"

/*!
 \class tst_QAbstractXmlForwardIterator
 \internal
 \since 4.4
 \brief Tests class QAbstractXmlForwardIterator.

 This test is not intended for testing the engine, but the functionality specific
 to the QAbstractXmlForwardIterator class.

 In other words, if you have an engine bug; don't add it here because it won't be
 tested properly. Instead add it to the test suite.

 */
class tst_QAbstractXmlForwardIterator : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void objectSize() const;
};

void tst_QAbstractXmlForwardIterator::objectSize() const
{
    /* We add sizeof(void *) times two. One for the
     * virtual table pointer, one for the d-pointer.
     *
     * The reason we check also allow (+ sizeof(void *) / 2) is that on 64 bit
     * the first member in QAbstractXmlForwardIterator gets padded to a pointer
     * address due to QSharedData not being sizeof(qptrdiff), but sizeof(qint32).
     */
    QVERIFY(   sizeof(QAbstractXmlForwardIterator<int>) == sizeof(QSharedData) + sizeof(void *) * 2
            || sizeof(QAbstractXmlForwardIterator<int>) == sizeof(QSharedData) + sizeof(void *) * 2 + sizeof(void *) / 2);
}

QTEST_MAIN(tst_QAbstractXmlForwardIterator)

#include "tst_qabstractxmlforwarditerator.moc"
#else //QTEST_XMLPATTERNS
QTEST_NOOP_MAIN
#endif
