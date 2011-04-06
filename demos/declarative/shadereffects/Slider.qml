/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Declarative module of the Qt Toolkit.
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

import QtQuick 2.0

Item {
    property real value: bar.x / (foo.width - bar.width)
    Item {
        id: foo
        width: parent.width - 4
        height: 6
        anchors.centerIn: parent

        Rectangle {
            height: parent.height
            anchors.left: parent.left
            anchors.right: bar.horizontalCenter
            color: "blue"
            radius: 3
        }
        Rectangle {
            height: parent.height
            anchors.left: bar.horizontalCenter
            anchors.right: parent.right
            color: "gray"
            radius: 3
        }
        Rectangle {
            anchors.fill: parent
            color: "transparent"
            radius: 3
            border.width: 2
            border.color: "black"
        }

        Rectangle {
            id: bar
            y: -7
            width: 20
            height: 20
            radius: 15
            color: "white"
            border.width: 2
            border.color: "black"
            MouseArea {
                anchors.fill: parent
                drag.target: parent
                drag.axis: Drag.XAxis
                drag.minimumX: 0
                drag.maximumX: foo.width - parent.width
            }
        }
    }
}

