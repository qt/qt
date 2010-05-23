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
This test shows a green rectangle moving and growing from the upper-left corner
of the black rectangle to the same position as the red rectangle (it should end up
the same height as the red rect and twice as wide). There should be no odd jumps or clipping seen.

The test shows one full transition (to the red and back), then several partial transitions, and
then a final full transition.
*/

Rectangle {
    width: 800;
    height: 480;
    color: "black";

    Rectangle {
        id: gr
        color: "green"
        width: 100; height: 100
    }

    MouseArea {
        id: mouser
        anchors.fill: parent
    }

    Rectangle {
        id: np
        x: 300
        width: 300; height: 300
        color: "yellow"
        clip: true
        Rectangle {
            color: "red"
            x: 100; y: 100; height: 100; width: 100
        }

    }

    Rectangle {
        id: vp
        x: 200; y: 200
        width: 100; height: 100
        color: "blue"
        rotation: 45
        scale: 2
    }

    states: State {
        name: "state1"
        when: mouser.pressed
        ParentChange {
            target: gr
            parent: np
            x: 100; y: 100; width: 200;
        }
    }

    transitions: Transition {
        reversible: true
        to: "state1"
        ParentAnimation {
            target: gr; via: vp;
            NumberAnimation { properties: "x,y,rotation,scale,width" }
        }
    }
}
