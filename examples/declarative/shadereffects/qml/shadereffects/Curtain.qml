/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 1.0
import Qt.labs.shaders 1.0

Item {
    id: main
    anchors.fill: parent

    Rectangle{
        id: bg
        anchors.fill: parent
        color: "black"
    }

    Image {
        source: "images/qt-logo.png"
        anchors.centerIn: parent
    }

    Image {
        id: fabric
        anchors.fill: parent
        source: "images/fabric.jpg"
        fillMode: Image.Tile
    }

    CurtainEffect {
        id: curtain
        anchors.fill: fabric
        bottomWidth: topWidth
        source: ShaderEffectSource { sourceItem: fabric; hideSource: true }

        Behavior on bottomWidth {
            SpringAnimation { easing.type: Easing.OutElastic; velocity: 250; mass: 1.5; spring: 0.5; damping: 0.05}
        }

        SequentialAnimation on topWidth {
            id: topWidthAnim
            loops: Animation.Infinite

            NumberAnimation { to: 360; duration: 1000 }
            PauseAnimation { duration: 2000 }
            NumberAnimation { to: 180; duration: 1000 }
            PauseAnimation { duration: 2000 }
        }
    }

    MouseArea {
        anchors.fill: parent

        onPressed: {
            topWidthAnim.stop()
            curtain.topWidth = mouseX;
        }

        onReleased: {
            topWidthAnim.restart()
        }

        onPositionChanged: {
            if (pressed) {
                curtain.topWidth = mouseX;
            }
        }
    }
}
