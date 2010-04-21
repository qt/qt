import Qt 4.7
import "content"
import "../scrollbar"

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
                    transformOrigin: Item.Center

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
                    transformOrigin: Item.Center

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
        orientation: "Vertical"
        position: view.visibleArea.yPosition
        pageSize: view.visibleArea.heightRatio

        // Only show the scrollbar when the view is moving.
        states: State {
            name: "ShowBars"; when: view.moving
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
