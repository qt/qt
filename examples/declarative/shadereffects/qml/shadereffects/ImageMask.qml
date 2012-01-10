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
    anchors.fill: parent

    Image {
        id: bg
        anchors.fill: parent
        source: "images/image2.jpg"
    }

    Item {
        id: mask
        anchors.fill: parent

        Text {
            text: "Mask text"
            font.pixelSize: 50
            font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter

            NumberAnimation on rotation {
                running: true
                loops: Animation.Infinite
                from: 0
                to: 360
                duration: 3000
            }

            SequentialAnimation on y {
                running: true
                loops: Animation.Infinite
                NumberAnimation {
                    to: main.height
                    duration: 3000
                }
                NumberAnimation {
                    to: 0
                    duration: 3000
                }
            }
        }

        Rectangle {
            id: opaqueBox
            width: 60
            height: parent.height
            SequentialAnimation on x {
                running: true
                loops: Animation.Infinite
                NumberAnimation {
                    to: main.width
                    duration: 2000
                    easing.type: Easing.InOutCubic
                }
                NumberAnimation {
                    to: 0
                    duration: 2000
                    easing.type: Easing.InOutCubic
                }
            }
        }

        Rectangle {
            width: 100
            opacity: 0.5
            height: parent.height
            SequentialAnimation on x {
                PauseAnimation { duration: 100 }

                SequentialAnimation {
                    loops: Animation.Infinite
                    NumberAnimation {
                        to: main.width
                        duration: 2000
                        easing.type: Easing.InOutCubic
                    }
                    NumberAnimation {
                        to: 0
                        duration: 2000
                        easing.type: Easing.InOutCubic
                    }
                }
            }
        }
    }

    ImageMaskEffect {
        anchors.fill: parent
        image: ShaderEffectSource {
            sourceItem: Image { source: "images/image1.jpg" }
            live: false
            hideSource: true
        }
        mask: ShaderEffectSource {
            sourceItem: mask
            live: true
            hideSource: true
        }
    }
}
