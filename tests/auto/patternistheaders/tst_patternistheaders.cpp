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
/*!
 \class tst_PatternistHeaders
 \internal
 \short
 \since 4.4
 \brief Tests that the expected headers are available for Patternist.

 This test is essentially a compilation test. It includes all the headers that are available for
 Patternist, and ensures it compiles.

 This attempts to capture regressions in header generation.
 */
class tst_PatternistHeaders : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void run() const;
};

void tst_PatternistHeaders::run() const
{
    /* This is a dummy, essentially. It only exists such that QTestLib
     * considers everything fine. */
}

/* If you've added a header, you need to add it four times. Twice in this list,
 * and twice in the one below. Alphabetic order. */
#include <QAbstractMessageHandler>
#include <qabstractmessagehandler.h>
#include <QAbstractUriResolver>
#include <qabstracturiresolver.h>
#include <QAbstractXmlNodeModel>
#include <qabstractxmlnodemodel.h>
#include <QAbstractXmlReceiver>
#include <qabstractxmlreceiver.h>
#include <QSimpleXmlNodeModel>
#include <qsimplexmlnodemodel.h>
#include <QSourceLocation>
#include <qsourcelocation.h>
#include <QtXmlPatterns>
#include <QXmlItem>
#include <QXmlName>
#include <qxmlname.h>
#include <QXmlNamePool>
#include <qxmlnamepool.h>
#include <QXmlNodeModelIndex>
#include <QXmlQuery>
#include <qxmlquery.h>
#include <QXmlResultItems>
#include <qxmlresultitems.h>
#include <QXmlSchema>
#include <qxmlschema.h>
#include <QXmlSchemaValidator>
#include <qxmlschemavalidator.h>
#include <QXmlSerializer>
#include <qxmlserializer.h>

/* Same again, this time with QtXmlPatterns prepended. Alphabetic order. */

#include <QtXmlPatterns/QAbstractMessageHandler>
#include <QtXmlPatterns/qabstractmessagehandler.h>
#include <QtXmlPatterns/QAbstractUriResolver>
#include <QtXmlPatterns/qabstracturiresolver.h>
#include <QtXmlPatterns/QAbstractXmlNodeModel>
#include <QtXmlPatterns/qabstractxmlnodemodel.h>
#include <QtXmlPatterns/QAbstractXmlReceiver>
#include <QtXmlPatterns/qabstractxmlreceiver.h>
#include <QtXmlPatterns/QSimpleXmlNodeModel>
#include <QtXmlPatterns/qsimplexmlnodemodel.h>
#include <QtXmlPatterns/QSourceLocation>
#include <QtXmlPatterns/qsourcelocation.h>
#include <QtXmlPatterns/QtXmlPatterns>
#include <QtXmlPatterns/QXmlItem>
#include <QtXmlPatterns/QXmlName>
#include <QtXmlPatterns/qxmlname.h>
#include <QtXmlPatterns/QXmlNamePool>
#include <QtXmlPatterns/qxmlnamepool.h>
#include <QtXmlPatterns/QXmlNodeModelIndex>
#include <QtXmlPatterns/QXmlQuery>
#include <QtXmlPatterns/qxmlquery.h>
#include <QtXmlPatterns/QXmlResultItems>
#include <QtXmlPatterns/qxmlresultitems.h>
#include <QtXmlPatterns/QXmlSchema>
#include <QtXmlPatterns/qxmlschema.h>
#include <QtXmlPatterns/QXmlSchemaValidator>
#include <QtXmlPatterns/qxmlschemavalidator.h>
#include <QtXmlPatterns/QXmlSerializer>
#include <QtXmlPatterns/qxmlserializer.h>

QTEST_MAIN(tst_PatternistHeaders)

#include "tst_patternistheaders.moc"
#else
QTEST_NOOP_MAIN
#endif

// vim: et:ts=4:sw=4:sts=4
