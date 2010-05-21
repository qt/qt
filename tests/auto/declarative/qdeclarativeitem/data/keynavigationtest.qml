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

Grid {
    columns: 2
    width: 100; height: 100
    Rectangle {
        id: item1
        objectName: "item1"
        focus: true
        width: 50; height: 50
        color: focus ? "red" : "lightgray"
        KeyNavigation.right: item2
        KeyNavigation.down: item3
        KeyNavigation.tab: item2
        KeyNavigation.backtab: item4
    }
    Rectangle {
        id: item2
        objectName: "item2"
        width: 50; height: 50
        color: focus ? "red" : "lightgray"
        KeyNavigation.left: item1
        KeyNavigation.down: item4
        KeyNavigation.tab: item3
        KeyNavigation.backtab: item1
    }
    Rectangle {
        id: item3
        objectName: "item3"
        width: 50; height: 50
        color: focus ? "red" : "lightgray"
        KeyNavigation.right: item4
        KeyNavigation.up: item1
        KeyNavigation.tab: item4
        KeyNavigation.backtab: item2
    }
    Rectangle {
        id: item4
        objectName: "item4"
        width: 50; height: 50
        color: focus ? "red" : "lightgray"
        KeyNavigation.left: item3
        KeyNavigation.up: item2
        KeyNavigation.tab: item1
        KeyNavigation.backtab: item3
    }
}
