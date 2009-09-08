import Qt 4.6

Item {
    ListModel {
        id: FruitModel
        ListElement {
            name: "Apple"
            cost: 2.45
        }
        ListElement {
            name: "Orange"
            cost: 3.25
        }
        ListElement {
            name: "Banana"
            cost: 1.95
        }
    }

    Component {
        id: FruitDelegate
        Item {
            width: 200; height: 50
            Text { text: name }
            Text { text: '$'+cost; anchors.right: parent.right }
            MouseRegion {
                anchors.fill: parent
                onClicked: FruitModel.remove(index)
            }
        }
    }

    ListView {
        model: FruitModel
        delegate: FruitDelegate
        anchors.fill: parent
    }
}
