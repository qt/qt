/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt 4.7
import "content"
import "../../ui-components/scrollbar"

Rectangle {
    id: container
    width: 640; height: 480
    color: "#343434"

    ListModel {
        id: fruitModel

        ListElement {
            name: "Apple"; cost: 2.45
            attributes: [
                ListElement { description: "Core" },
                ListElement { description: "Deciduous" }
            ]
        }
        ListElement {
            name: "Banana"; cost: 1.95
            attributes: [
                ListElement { description: "Tropical" },
                ListElement { description: "Seedless" }
            ]
        }
        ListElement {
            name: "Cumquat"; cost: 3.25
            attributes: [
                ListElement { description: "Citrus" }
            ]
        }
        ListElement {
            name: "Durian"; cost: 9.95
            attributes: [
                ListElement { description: "Tropical" },
                ListElement { description: "Smelly" }
            ]
        }
        ListElement {
            name: "Elderberry"; cost: 0.05
            attributes: [
                ListElement { description: "Berry" }
            ]
        }
        ListElement {
            name: "Fig"; cost: 0.25
            attributes: [
                ListElement { description: "Flower" }
            ]
        }
    }

    Component {
        id: fruitDelegate

        Item {
            width: container.width; height: 55

            Column {
                id: moveButtons
                x: 5; width: childrenRect.width; anchors.verticalCenter: parent.verticalCenter

                Image {
                    source: "content/pics/go-up.png"
                    MouseArea { anchors.fill: parent; onClicked: fruitModel.move(index,index-1,1) }
                }
                Image { source: "content/pics/go-down.png"
                    MouseArea { anchors.fill: parent; onClicked: fruitModel.move(index,index+1,1) }
                }
            }

            Column {
                anchors { right: itemButtons.left; verticalCenter: parent.verticalCenter; left: moveButtons.right; leftMargin: 10 }

                Text {
                    id: label
                    width: parent.width
                    color: "White"
                    font.bold: true; font.pixelSize: 15
                    text: name; elide: Text.ElideRight
                }
                Row {
                    spacing: 5
                    Repeater {
                        model: attributes
                        Component {
                            Text { text: description; color: "White" }
                        }
                    }
                }
            }

            Row {
                id: itemButtons

                anchors { right: removeButton.left; rightMargin: 35; verticalCenter: parent.verticalCenter }
                width: childrenRect.width
                spacing: 10

                Image {
                    source: "content/pics/list-add.png"
                    scale: clickUp.isPressed ? 0.9 : 1

                    ClickAutoRepeating {
                        id: clickUp
                        anchors.fill: parent
                        onClicked: fruitModel.setProperty(index, "cost", cost+0.25)
                    }
                }

                Text { id: costText; text: '$'+Number(cost).toFixed(2); font.pixelSize: 15; color: "White"; font.bold: true; }

                Image {
                    source: "content/pics/list-remove.png"
                    scale: clickDown.isPressed ? 0.9 : 1

                    ClickAutoRepeating {
                        id: clickDown
                        anchors.fill: parent
                        onClicked: fruitModel.setProperty(index, "cost", Math.max(0,cost-0.25))
                    }
                }
            }
            Image {
                id: removeButton
                anchors { verticalCenter: parent.verticalCenter; right: parent.right; rightMargin: 10 }
                source: "content/pics/archive-remove.png"

                MouseArea { anchors.fill:parent; onClicked: fruitModel.remove(index) }
            }
        }
    }

    ListView {
        id: view
        anchors { top: parent.top; left: parent.left; right: parent.right; bottom: buttons.top }
        model: fruitModel
        delegate: fruitDelegate
    }

    // Attach scrollbar to the right edge of the view.
    ScrollBar {
        id: verticalScrollBar

        width: 8; height: view.height; anchors.right: view.right
        opacity: 0
        orientation: Qt.Vertical
        position: view.visibleArea.yPosition
        pageSize: view.visibleArea.heightRatio

        // Only show the scrollbar when the view is moving.
        states: State {
            name: "ShowBars"; when: view.movingVertically
            PropertyChanges { target: verticalScrollBar; opacity: 1 }
        }
        transitions: Transition {
            NumberAnimation { properties: "opacity"; duration: 400 }
        }
    }

    Row {
        id: buttons

        x: 8; width: childrenRect.width; height: childrenRect.height
        anchors { bottom: parent.bottom; bottomMargin: 8 }
        spacing: 8

        Image {
            source: "content/pics/archive-insert.png"

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    fruitModel.append({
                        "name": "Pizza Margarita",
                        "cost": 5.95,
                        "attributes": [{"description": "Cheese"},{"description": "Tomato"}]
                    })
                }
            }
        }

        Image {
            source: "content/pics/archive-insert.png"

            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    fruitModel.insert(0, {
                        "name": "Pizza Supreme",
                        "cost": 9.95,
                        "attributes": [{"description": "Cheese"},{"description": "Tomato"},{"description": "The Works"}]
                    })
                }
            }
        }

        Image {
            source: "content/pics/archive-remove.png"

            MouseArea {
                anchors.fill: parent
                onClicked: fruitModel.clear()
            }
        }
    }
}
