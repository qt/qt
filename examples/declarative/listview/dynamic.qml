import Qt 4.6
import "content"

Item {
    width: 320
    height: 500

    ListModel {
        id: FruitModel
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
        id: FruitDelegate
        Item {
            width: parent.width; height: 55
            Text { id: Label; font.pixelSize: 24; text: name; elide: "ElideRight"; anchors.right: Cost.left; anchors.left:parent.left }
            Text { id: Cost; font.pixelSize: 24; text: '$'+Number(cost).toFixed(2); anchors.right: ItemButtons.left }
            Row { 
                anchors.top: Label.bottom
                spacing: 5
                Repeater {
                    model: attributes
                    Component {
                        Text { text: description }
                    }
                }
            }
            Row {
                id: ItemButtons
                anchors.right: parent.right
                width: childrenRect.width
                Image { source: "content/pics/add.png"
                    ClickAutoRepeating { id: ClickUp; anchors.fill: parent; onClicked: FruitModel.set(index,"cost",Number(cost)+0.25) }
                    scale: ClickUp.pressed ? 0.9 : 1
                }
                Image { source: "content/pics/del.png"
                    ClickAutoRepeating { id: ClickDown; anchors.fill: parent; onClicked: FruitModel.set(index,"cost",Math.max(0,Number(cost)-0.25)) }
                    scale: ClickDown.pressed ? 0.9 : 1
                }
                Image { source: "content/pics/trash.png"
                    MouseRegion { anchors.fill: parent; onClicked: FruitModel.remove(index) }
                }
                Column {
                    width: childrenRect.width
                    Image { source: "content/pics/moreUp.png"
                        MouseRegion { anchors.fill: parent; onClicked: FruitModel.move(index,index-1,1) }
                    }
                    Image { source: "content/pics/moreDown.png"
                        MouseRegion { anchors.fill: parent; onClicked: FruitModel.move(index,index+1,1) }
                    }
                }
            }
        }
    }

    ListView {
        model: FruitModel
        delegate: FruitDelegate
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: Buttons.top
    }

    Row {
        width: childrenRect.width
        height: childrenRect.height
        anchors.bottom: parent.bottom
        id: Buttons
        Image { source: "content/pics/add.png"
            MouseRegion { anchors.fill: parent;
                onClicked: {
                    FruitModel.append({
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
                    FruitModel.insert(0,{
                        "name":"Pizza Supreme",
                        "cost":9.95,
                        "attributes":[{"description": "Cheese"},{"description": "Tomato"},{"description": "The Works"}]
                    })
                }
            }
        }
        Image { source: "content/pics/trash.png"
            MouseRegion { anchors.fill: parent; onClicked: FruitModel.clear() }
        }
    }
}
