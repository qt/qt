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

Row {

//![transition]
Item {
    width: 400; height: 400

    Rectangle {
        id: rect
        width: 200; height: 100
        color: "red"

        states: State {
            name: "rotated"
            PropertyChanges { target: rect; rotation: 180; transformOrigin: Item.BottomRight }
        }

        transitions: Transition {
            RotationAnimation { duration: 1000; direction: RotationAnimation.Counterclockwise }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: rect.state = "rotated"
        }
    }
}
//![transition]

Item {
    width: 300; height: 300

    Image { id: img; source: "pics/qt.png" }

//![standalone]
SequentialAnimation {
    PropertyAction { target: img; property: "smooth"; value: "true" }
    NumberAnimation { target: img; property: "width"; to: 300; duration: 1000 }
    PropertyAction { target: img; property: "smooth"; value: "false" }
}
//![standalone]
}

}

