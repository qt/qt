/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtTest module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qtestfilelogger.h"
#include "qtestassert.h"
#include "QtTest/private/qtestlog_p.h"
#include "QtTest/private/qtestresult_p.h"

#include <QtCore/qdir.h>

#include <stdlib.h>
#include <stdio.h>

QT_BEGIN_NAMESPACE

namespace QTest
{
    static FILE *stream = 0;
}

QTestFileLogger::QTestFileLogger()
{
}

QTestFileLogger::~QTestFileLogger()
{
    if(QTest::stream)
        fclose(QTest::stream);

    QTest::stream = 0;
}

void QTestFileLogger::init()
{
    char filename[100];
    int index = 0;
#if defined(Q_OS_SYMBIAN)
    QByteArray ba(QDir::toNativeSeparators(QString(QDir::homePath()+QDir::separator())).toUtf8());
    index = ba.length();
    QTest::qt_snprintf(filename, sizeof(filename), "%s%s.log",
                ba.constData(), QTestResult::currentTestObjectName());
#else
    QTest::qt_snprintf(filename, sizeof(filename), "%s.log",
                QTestResult::currentTestObjectName());
#endif
 
     // Keep filenames simple
    for (uint i = index; i < sizeof(filename) && filename[i]; ++i) {
        char& c = filename[i];
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
              || (c >= '0' && c <= '9') || c == '-' || c == '.')) {
            c = '_';
        }
    }

#if defined(_MSC_VER) && _MSC_VER >= 1400 && !defined(Q_OS_WINCE)
    if (::fopen_s(&QTest::stream, filename, "wt")) {
#else
    QTest::stream = ::fopen(filename, "wt");
    if (!QTest::stream) {
#endif
        printf("Unable to open file for simple logging: %s", filename);
        ::exit(1);
    }
}

void QTestFileLogger::flush(const char *msg)
{
    QTEST_ASSERT(QTest::stream);

    ::fputs(msg, QTest::stream);
    ::fflush(QTest::stream);
}

QT_END_NAMESPACE

