import Qt 4.6
import "content"

Item {
    width: 320
    height: 500

    ListModel {
        id: fruitModel
        ListElement {
            name: "Apple"
            cost: 2.45
            attributes: [
                ListElement { description: "Core" },
                ListElement { description: "Deciduous" }
            ]
        }
        ListElement {
            name: "Banana"
            cost: 1.95
            attributes: [
                ListElement { description: "Tropical" },
                ListElement { description: "Seedless" }
            ]
        }
        ListElement {
            name: "Cumquat"
            cost: 3.25
            types: [ "Small", "Smaller" ]
            attributes: [
                ListElement { description: "Citrus" }
            ]
        }
        ListElement {
            name: "Durian"
            cost: 9.95
            attributes: [
                ListElement { description: "Tropical" },
                ListElement { description: "Smelly" }
            ]
        }
        ListElement {
            name: "Elderberry"
            cost: 0.05
            attributes: [
                ListElement { description: "Berry" }
            ]
        }
        ListElement {
            name: "Fig"
            cost: 0.25
            attributes: [
                ListElement { description: "Flower" }
            ]
        }
    }

    Component {
        id: fruitDelegate
        Item {
            width: parent.width; height: 55
            Text { id: label; font.pixelSize: 24; text: name; elide: "ElideRight"; anchors.right: cost.left; anchors.left:parent.left }
            Text { id: cost; font.pixelSize: 24; text: '$'+Number(cost).toFixed(2); anchors.right: itemButtons.left }
            Row {
                anchors.top: label.bottom
                spacing: 5
                Repeater {
                    model: attributes
                    Component {
                        Text { text: description }
                    }
                }
            }
            Row {
                id: itemButtons
                anchors.right: parent.right
                width: childrenRect.width
                Image { source: "content/pics/add.png"
                    ClickAutoRepeating { id: clickUp; anchors.fill: parent; onClicked: fruitModel.set(index,"cost",Number(cost)+0.25) }
                    scale: clickUp.pressed ? 0.9 : 1
                }
                Image { source: "content/pics/del.png"
                    ClickAutoRepeating { id: clickDown; anchors.fill: parent; onClicked: fruitModel.set(index,"cost",Math.max(0,Number(cost)-0.25)) }
                    scale: clickDown.pressed ? 0.9 : 1
                }
                Image { source: "content/pics/trash.png"
                    MouseRegion { anchors.fill: parent; onClicked: fruitModel.remove(index) }
                }
                Column {
                    width: childrenRect.width
                    Image { source: "content/pics/moreUp.png"
                        MouseRegion { anchors.fill: parent; onClicked: fruitModel.move(index,index-1,1) }
                    }
                    Image { source: "content/pics/moreDown.png"
                        MouseRegion { anchors.fill: parent; onClicked: fruitModel.move(index,index+1,1) }
                    }
                }
            }
        }
    }

    ListView {
        model: fruitModel
        delegate: fruitDelegate
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: buttons.top
    }

    Row {
        width: childrenRect.width
        height: childrenRect.height
        anchors.bottom: parent.bottom
        id: buttons
        Image { source: "content/pics/add.png"
            MouseRegion { anchors.fill: parent;
                onClicked: {
                    fruitModel.append({
                        "name":"Pizza Margarita",
                        "cost":5.95,
                        "attributes":[{"description": "Cheese"},{"description": "Tomato"}]
                    })
                }
            }
        }
        Image { source: "content/pics/add.png"
            MouseRegion { anchors.fill: parent;
                onClicked: {
                    fruitModel.insert(0,{
                        "name":"Pizza Supreme",
                        "cost":9.95,
                        "attributes":[{"description": "Cheese"},{"description": "Tomato"},{"description": "The Works"}]
                    })
                }
            }
        }
        Image { source: "content/pics/trash.png"
            MouseRegion { anchors.fill: parent; onClicked: fruitModel.clear() }
        }
    }
}
