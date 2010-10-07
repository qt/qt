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

import QtQuick 1.0
import "../common" as Common

Flipable {
    id: container

    property alias frontContainer: containerFront
    property string photoTitle: ""
    property string photoTags: ""
    property int photoWidth
    property int photoHeight
    property string photoType
    property string photoAuthor
    property string photoDate
    property string photoUrl
    property int rating: 2
    property variant prevScale: 1.0

    signal closed

    transform: Rotation {
        id: itemRotation
        origin.x: container.width / 2;
        axis.y: 1; axis.z: 0
    }

    front: Item {
        id: containerFront; anchors.fill: container

        Rectangle {
            anchors.fill: parent
            color: "black"; opacity: 0.4
        }

        Column {
            spacing: 10
            anchors {
                left: parent.left; leftMargin: 10
                right: parent.right; rightMargin: 10
                top: parent.top; topMargin: 120
            }
            Text { font.bold: true; color: "white"; elide: Text.ElideRight; text: container.photoTitle; width: parent.width }
            Text { color: "white"; elide: Text.ElideRight; text: "Size: " + container.photoWidth + 'x' + container.photoHeight; width: parent.width }
            Text { color: "white"; elide: Text.ElideRight; text: "Type: " + container.photoType; width: parent.width }
            Text { color: "white"; elide: Text.ElideRight; text: "Author: " + container.photoAuthor; width: parent.width }
            Text { color: "white"; elide: Text.ElideRight; text: "Published: " + container.photoDate; width: parent.width }
            Text { color: "white"; elide: Text.ElideRight; text: container.photoTags == "" ? "" : "Tags: "; width: parent.width }
            Text { color: "white"; elide: Text.ElideRight; text: container.photoTags; width: parent.width }
        }
    }

    back: Item {
        anchors.fill: container

        Rectangle { anchors.fill: parent; color: "black"; opacity: 0.4 }

        Common.Progress {
            anchors.centerIn: parent; width: 200; height: 22
            progress: bigImage.progress; visible: bigImage.status != Image.Ready
        }

        Flickable {
            id: flickable; anchors.fill: parent; clip: true
            contentWidth: imageContainer.width; contentHeight: imageContainer.height

            function updateMinimumScale() {
                if (bigImage.status == Image.Ready && bigImage.width != 0) {
                    slider.minimum = Math.min(flickable.width / bigImage.width, flickable.height / bigImage.height);
                    if (bigImage.width * slider.value > flickable.width) {
                        var xoff = (flickable.width/2 + flickable.contentX) * slider.value / prevScale;
                        flickable.contentX = xoff - flickable.width/2;
                    }
                    if (bigImage.height * slider.value > flickable.height) {
                        var yoff = (flickable.height/2 + flickable.contentY) * slider.value / prevScale;
                        flickable.contentY = yoff - flickable.height/2;
                    }
                    prevScale = slider.value;
                }
            }

            onWidthChanged: updateMinimumScale()
            onHeightChanged: updateMinimumScale()

            Item {
                id: imageContainer
                width: Math.max(bigImage.width * bigImage.scale, flickable.width);
                height: Math.max(bigImage.height * bigImage.scale, flickable.height);

                Image {
                    id: bigImage; source: container.photoUrl; scale: slider.value
                    anchors.centerIn: parent; smooth: !flickable.movingVertically
                    onStatusChanged : {
                        // Default scale shows the entire image.
                        if (bigImage.status == Image.Ready && bigImage.width != 0) {
                            slider.minimum = Math.min(flickable.width / bigImage.width, flickable.height / bigImage.height);
                            prevScale = Math.min(slider.minimum, 1);
                            slider.value = prevScale;
                        }
                    }
                }
            }
        }

        Text {
            text: "Image Unavailable"
            visible: bigImage.status == Image.Error
            anchors.centerIn: parent; color: "white"; font.bold: true
        }

        Common.Slider {
            id: slider; visible: { bigImage.status == Image.Ready && maximum > minimum }
            anchors {
                bottom: parent.bottom; bottomMargin: 65
                left: parent.left; leftMargin: 25
                right: parent.right; rightMargin: 25
            }
            onValueChanged: {
                if (bigImage.width * value > flickable.width) {
                    var xoff = (flickable.width/2 + flickable.contentX) * value / prevScale;
                    flickable.contentX = xoff - flickable.width/2;
                }
                if (bigImage.height * value > flickable.height) {
                    var yoff = (flickable.height/2 + flickable.contentY) * value / prevScale;
                    flickable.contentY = yoff - flickable.height/2;
                }
                prevScale = value;
            }
        }
    }

    states: State {
        name: "Back"
        PropertyChanges { target: itemRotation; angle: 180 }
        PropertyChanges { target: toolBar; button2Visible: false }
        PropertyChanges { target: toolBar; button1Label: "Back" }
    }

    transitions: Transition {
        SequentialAnimation {
            PropertyAction { target: bigImage; property: "smooth"; value: false }
            NumberAnimation { easing.type: Easing.InOutQuad; properties: "angle"; duration: 500 }
            PropertyAction { target: bigImage; property: "smooth"; value: !flickable.movingVertically }
        }
    }
}
