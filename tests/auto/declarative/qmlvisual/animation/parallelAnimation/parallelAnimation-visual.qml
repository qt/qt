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

/*
    This test verifies that a single animation animating two properties is visually the same as two
    animations in a parallel group animating the same properties. Visually, you should see a red
    rectangle at 0,0 stretching from the top of the window to the bottom. This rect will be moved to
    the right side of the window while turning purple. If the bottom half is visually different
    than the top half, there is a problem.
*/

Rectangle {
    width: 400; height: 200
    Rectangle {
        id: redRect
        width: 100; height: 100
        color: "red"
    }
    Rectangle {
        id: redRect2
        width: 100; height: 100
        y: 100
        color: "red"
    }

    MouseArea {
        anchors.fill: parent
        onClicked: parent.state = "state1"
    }

    states: State {
        name: "state1"
        PropertyChanges {
            target: redRect
            x: 300
            color: "purple"
        }
        PropertyChanges {
            target: redRect2
            x: 300
            color: "purple"
        }
    }

    transitions: Transition {
        PropertyAnimation { targets: redRect; properties: "x,color"; duration: 300 }
        ParallelAnimation {
            NumberAnimation { targets: redRect2; properties: "x"; duration: 300 }
            ColorAnimation { targets: redRect2; properties: "color"; duration: 300 }
        }
    }
}
