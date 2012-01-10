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
    width: 360
    height: 640

    Rectangle {
        anchors.fill: parent
        color: "black"
    }

    Image {
        id: header
        source: "images/shader_effects.jpg"
    }

    ListModel {
        id: demoModel
        ListElement { name: "ImageMask" }
        ListElement { name: "RadialWave" }
        ListElement { name: "Water" }
        ListElement { name: "Grayscale" }
        ListElement { name: "DropShadow" }
        ListElement { name: "Curtain" }
    }

    Grid {
        id: menuGrid
        anchors.top: header.bottom
        anchors.bottom: toolbar.top
        width: parent.width
        columns: 2
        Repeater {
            model: demoModel
            Item {
                width: main.width / 2
                height: menuGrid.height / 3
                clip: true
                Image {
                    width: parent.width
                    height: width
                    source: "images/" + name + ".jpg"
                    opacity: mouseArea.pressed ? 0.6 : 1.0
                }
                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    onClicked: {
                        demoLoader.source = name + ".qml"
                        main.state = "showDemo"
                    }
                }
            }
        }
    }

    Loader {
        anchors.fill: parent
        id: demoLoader
        visible: false
        Behavior on opacity {
            NumberAnimation { duration: 300 }
        }
    }

    Image {
        id: toolbar
        source: "images/toolbar.png"
        width: parent.width
        anchors.bottom: parent.bottom
    }

    Rectangle {
        id: translucentToolbar
        color: "black"
        opacity: 0.3
        anchors.fill: toolbar
        visible: !toolbar.visible
    }

    Item {
        height: toolbar.height
        width: height
        anchors.bottom: parent.bottom

        Image {
            source: "images/back.png"
            anchors.centerIn: parent
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (main.state == "") Qt.quit(); else {
                    main.state = ""
                    demoLoader.source = ""
                }
            }
        }
    }

    states: State {
        name: "showDemo"
        PropertyChanges {
            target: menuGrid
            visible: false
        }
        PropertyChanges {
            target: demoLoader
            visible: true
        }
        PropertyChanges {
            target: toolbar
            visible: false
        }
    }
}
