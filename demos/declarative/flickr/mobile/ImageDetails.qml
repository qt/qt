/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt 4.7
import "../common" as Common

Flipable {
    id: container

    property variant frontContainer: containerFront
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
                left: parent.left; leftMargin: 20
                right: parent.right; rightMargin: 20
                top: parent.top; topMargin: 180
            }
            Text { font.bold: true; color: "white"; elide: Text.ElideRight; text: container.photoTitle }
            Text { color: "white"; elide: Text.ElideRight; text: "<b>Size:</b> " + container.photoWidth + 'x' + container.photoHeight }
            Text { color: "white"; elide: Text.ElideRight; text: "<b>Type:</b> " + container.photoType }
            Text { color: "white"; elide: Text.ElideRight; text: "<b>Author:</b> " + container.photoAuthor }
            Text { color: "white"; elide: Text.ElideRight; text: "<b>Published:</b> " + container.photoDate }
            Text { color: "white"; elide: Text.ElideRight; text: container.photoTags == "" ? "" : "<b>Tags:</b> " }
            Text { color: "white"; elide: Text.ElideRight; text: container.photoTags }
        }
    }

    back: Item {
        anchors.fill: container

        Rectangle { anchors.fill: parent; color: "black"; opacity: 0.4 }

        Common.Progress {
            anchors.centerIn: parent; width: 200; height: 18
            progress: bigImage.progress; visible: bigImage.status != Image.Ready
        }

        Flickable {
            id: flickable; anchors.fill: parent; clip: true
            contentWidth: imageContainer.width; contentHeight: imageContainer.height

            Item {
                id: imageContainer
                width: Math.max(bigImage.width * bigImage.scale, flickable.width);
                height: Math.max(bigImage.height * bigImage.scale, flickable.height);

                Image {
                    id: bigImage; source: container.photoUrl; scale: slider.value
                    anchors.centerIn: parent; smooth: !flickable.movingVertically
                    onStatusChanged : {
                        // Default scale shows the entire image.
                        if (status == Image.Ready && width != 0) {
                            slider.minimum = Math.min(flickable.width / width, flickable.height / height);
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
    }

    transitions: Transition {
        SequentialAnimation {
            PropertyAction { target: bigImage; property: "smooth"; value: false }
            NumberAnimation { easing.type: Easing.InOutQuad; properties: "angle"; duration: 500 }
            PropertyAction { target: bigImage; property: "smooth"; value: !flickable.movingVertically }
        }
    }
}
