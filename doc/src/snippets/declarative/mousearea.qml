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

//! [import]
import Qt 4.7
//! [import]

Rectangle {
    width: childrenRect.width
    height: childrenRect.height

Row {

//! [intro]
Rectangle { 
    width: 100; height: 100
    color: "green"

    MouseArea { 
        anchors.fill: parent
        onClicked: { parent.color = 'red' }
    }
}
//! [intro]

//! [intro-extended]
Rectangle {
    width: 100; height: 100
    color: "green"

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            if (mouse.button == Qt.RightButton)
                parent.color = 'blue';
            else
                parent.color = 'red';
        }
    }
}
//! [intro-extended]

//! [drag]
Rectangle {
    id: container
    width: 600; height: 200

    Image {
        id: pic
        source: "pics/qt.png"
        opacity: (600.0 - pic.x) / 600

        MouseArea {
            anchors.fill: parent
            drag.target: pic
            drag.axis: Drag.XAxis
            drag.minimumX: 0
            drag.maximumX: container.width - pic.width
        }
    }
}
//! [drag]

//! [mousebuttons]
Text {
    text: mouseArea.pressedButtons & Qt.RightButton ? "right" : ""
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
    }
}
//! [mousebuttons]

}

}
