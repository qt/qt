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

#include <QtTest/private/qbenchmarkmetric_p.h>

/*!
  \enum QTest::QBenchmarkMetric
  \since 4.7

  This enum lists all the things that can be benchmarked.
  
  \value FramesPerSecond        Frames per second
  \value BitsPerSecond          Bits per second
  \value BytesPerSecond         Bytes per second
  \value WalltimeMilliseconds   Clock time in milliseconds
  \value CPUTicks               CPU time
  \value InstructionReads       Instruction reads
  \value Events                 Event count

  \sa QTest::benchmarkMetricName(), QTest::benchmarkMetricUnit()

 */

/*!
  \relates QTest
  \since 4.7
  Returns the enum value \a metric as a character string.
 */
const char * QTest::benchmarkMetricName(QBenchmarkMetric metric)
{
    switch (metric) {
    case FramesPerSecond:
        return "FramesPerSecond";
    case BitsPerSecond:
        return "BitsPerSecond";
    case BytesPerSecond:
        return "BytesPerSecond";
    case WalltimeMilliseconds:
        return "WalltimeMilliseconds";
    case CPUTicks:
        return "CPUTicks";
    case InstructionReads:
        return "InstructionReads";
    case Events:
        return "Events";
    default:
        return "";
    }
};

/*!
  \relates QTest
  \since 4.7
  Retuns the units of measure for the specified \a metric.
 */
const char * QTest::benchmarkMetricUnit(QBenchmarkMetric metric)
{
    switch (metric) {
    case FramesPerSecond:
        return "fps";
    case BitsPerSecond:
        return "bits/s";
    case BytesPerSecond:
        return "bytes/s";
    case WalltimeMilliseconds:
        return "msecs";
    case CPUTicks:
        return "CPU ticks";
    case InstructionReads:
        return "instruction reads";
    case Events:
        return "events";
    default:
        return "";
    }
}

