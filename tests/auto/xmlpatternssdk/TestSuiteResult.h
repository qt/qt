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

#ifndef PatternistSDK_TestSuiteResult_H
#define PatternistSDK_TestSuiteResult_H

#include <QDate>
#include <QString>

#include "TestResult.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternistSDK
{
    /**
     * @short A collection of TestResult instances.
     *
     * A TestSuiteResult gathers all TestResult instances, and provides
     * the toXML() function which serializes it all into a XQuery Test Suite
     * result file, conforming to XQTSResult.xsd.
     *
     * @ingroup PatternistSDK
     * @author Frans Englich <frans.englich@nokia.com>
     */
    class Q_PATTERNISTSDK_EXPORT TestSuiteResult
    {
    public:
        ~TestSuiteResult();

        TestSuiteResult(const QString &testSuiteVersion,
                        const QDate &runDate,
                        const TestResult::List &results);

        /**
         * Serializes the test results this TestSuiteResult represents,
         * into XQTS test-suite-result document, conformant to XQTSCatalog.xsd.
         */
        void toXML(XMLWriter &receiver) const;

    private:
        const QString m_testSuiteVersion;
        const QDate m_runDate;
        const TestResult::List m_results;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
// vim: et:ts=4:sw=4:sts=4
