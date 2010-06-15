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

//![property-anim-1]
Rectangle {
    id: rect
    width: 120; height: 200

    Image {
        id: img
        source: "pics/qt.png"
        x: 60 - img.width/2
        y: 0

        SequentialAnimation on y {
            loops: Animation.Infinite
            NumberAnimation { to: 200 - img.height; easing.type: Easing.OutBounce; duration: 2000 }
            PauseAnimation { duration: 1000 }
            NumberAnimation { to: 0; easing.type: Easing.OutQuad; duration: 1000 }
        }
    }
}
//![property-anim-1]

//![property-anim-2]
Rectangle {
    width: 200; height: 200

    Rectangle {
        color: "red"
        width: 50; height: 50
        NumberAnimation on x { to: 50 }
    }
}
//![property-anim-2]


Item {
//![property-anim-3]
PropertyAnimation {
    id: animation
    target: image
    property: "scale"
    from: 1; to: 0.5
}

Image {
    id: image
    source: "pics/qt.png"
    MouseArea {
        anchors.fill: parent
        onPressed: animation.start()
    }
}
//![property-anim-3]
}


//![transitions-1]
transitions: [
    Transition {
        NumberAnimation {
            properties: "x,y"
            easing.type: Easing.OutBounce
            duration: 200
        }
    }
]
//![transitions-1]


//![transitions-2]
Transition {
    from: "*"
    to: "MyState"
    reversible: true

    SequentialAnimation {
        NumberAnimation {
            duration: 1000
            easing.type: Easing.OutBounce

            // animate myItem's x and y if they have changed in the state
            target: myItem
            properties: "x,y"
        }

        NumberAnimation {
            duration: 1000

            // animate myItem2's y to 200, regardless of what happens in the state
            target: myItem2
            property: "y"
            to: 200
        }
    }
}
//![transitions-2]


//![transitions-3]
Transition {
    from: "*"
    to: "MyState"
    reversible: true

    SequentialAnimation {
        ColorAnimation { duration: 1000 }
        PauseAnimation { duration: 1000 }

        ParallelAnimation {
            NumberAnimation {
                duration: 1000
                easing.type: Easing.OutBounce
                targets: box1
                properties: "x,y"
            }
            NumberAnimation {
                duration: 1000
                targets: box2
                properties: "x,y"
            }
        }
    }
}
//![transitions-3]

//![behavior]
Rectangle {
    id: redRect
    color: "red"
    width: 100; height: 100

    Behavior on x { 
        NumberAnimation { duration: 300; easing.type: Easing.InOutQuad }
    }
}
//![behavior]

}
