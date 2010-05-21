/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

import Qt 4.7

QtObject {
    property date date1: "2008-12-24"
    property string test1: Qt.formatDate(date1)
    property string test2: Qt.formatDate(date1, Qt.DefaultLocaleLongDate)
    property string test3: Qt.formatDate(date1, "ddd MMMM d yy")

    property variant time1: new Date(0,0,0,14,15,38,200)
    property string test4: Qt.formatTime(time1)
    property string test5: Qt.formatTime(time1, Qt.DefaultLocaleLongDate)
    property string test6: Qt.formatTime(time1, "H:m:s a")
    property string test7: Qt.formatTime(time1, "hh:mm:ss.zzz")

    property variant dateTime1: new Date(1978,2,4,9,13,54)
    property string test8: Qt.formatDateTime(dateTime1)
    property string test9: Qt.formatDateTime(dateTime1, Qt.DefaultLocaleLongDate)
    property string test10: Qt.formatDateTime(dateTime1, "M/d/yy H:m:s a")

    // Error cases
    property string test11: Qt.formatDate()
    property string test12: Qt.formatDate(new Date, new Object)

    property string test13: Qt.formatTime()
    property string test14: Qt.formatTime(new Date, new Object)

    property string test15: Qt.formatDateTime()
    property string test16: Qt.formatDateTime(new Date, new Object)
}
