/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QML Shaders plugin of the Qt Toolkit.
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

Rectangle {
    id :root
    anchors.fill: parent;
    color: "green"

    Timer {
        running: true
        interval: 2000
        repeat: true
        onTriggered: {
            effect1.visible = !effect1.visible
            effect2.visible = !effect2.visible
            //effect3.visible = !effect3.visible
        }
    }

    Rectangle {
        id: a
        x: 90
        y: 90
        color: "red"
        width: 220
        height: 220
        Rectangle {
            id: b
            x: 10
            y: 10
            color: "blue"
            width: 100
            height: 100
            rotation: 5
            Rectangle {
                id: c
                x: 10
                y: 10
                color: "black"
                width: 80
                height: 80
            }
        }
        Rectangle {
            id: d
            x: 10
            y: 110
            color: "yellow"
            width: 100
            height: 100
        }
    }

    ShaderEffectItem {
        id: effect1
        anchors.fill: a
        property variant source: ShaderEffectSource{ sourceItem: a; hideSource: true }
    }

    ShaderEffectItem {
        id: effect2
        x: 100
        y: 100
        width: 100
        height: 100
        rotation: 5
        property variant source: ShaderEffectSource{ sourceItem: b; hideSource: true }
    }

//    ShaderEffectItem {
//        id: effect3
//        x: 110
//        y: 210
//        width: 80
//        height: 80
//        property variant source: ShaderEffectSource{ sourceItem: c; hideSource: true }
//    }


    Rectangle {
        width: parent.width
        height: 40
        color: "#cc000000"

        Text {
             id: label
             anchors.centerIn:  parent
             text: effect1.visible ? "Effects active" : "Effects NOT active"
             color: "white"
             font.bold: true
        }
    }
}
