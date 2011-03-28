/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

import QtQuick 1.0

Item {
    id: root
    property int dynamicWidth: 100
    property int dynamicHeight: rect1.height + rect2.height
    property int widthSignaledProperty: 10
    property int heightSignaledProperty: 10

    Rectangle {
        id: rect1
        width: root.dynamicWidth + 20
        height: width + (5*3) - 8 + (width/9)
        color: "red"
        border.color: "black"
        border.width: 5
        radius: 10
    }

    Rectangle {
        id: rect2
        width: rect1.width - 50
        height: width + (5*4) - 6 + (width/3)
        color: "red"
        border.color: "black"
        border.width: 5
        radius: 10
    }

    onDynamicWidthChanged: {
        widthSignaledProperty = widthSignaledProperty + (6*5) - 2;
    }

    onDynamicHeightChanged: {
        heightSignaledProperty = widthSignaledProperty + heightSignaledProperty + (5*3) - 7;
    }
}
