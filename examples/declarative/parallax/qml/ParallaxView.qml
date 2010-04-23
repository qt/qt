import Qt 4.7

Item {
    id: root

    property alias background: background.source
    default property alias content: visualModel.children
    property int currentIndex: 0

    Image {
        id: background
        fillMode: Image.TileHorizontally
        x: -list.contentX / 2
        width: Math.max(list.contentWidth, parent.width)
    }

    ListView {
        id: list

        currentIndex: root.currentIndex
        onCurrentIndexChanged: root.currentIndex = currentIndex 

        orientation: "Horizontal"
        overShoot: false
        anchors.fill: parent
        model: VisualItemModel { id: visualModel }

        highlightRangeMode: ListView.StrictlyEnforceRange
        snapMode: ListView.SnapOneItem
    }

    ListView {
        id: selector

        Rectangle {
            color: "#60FFFFFF"
            x: -10; y: -10; radius: 10; z: -1
            width: parent.width + 20; height: parent.height + 20
        }
        currentIndex: root.currentIndex
        onCurrentIndexChanged: root.currentIndex = currentIndex

        height: 50
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: Math.min(count * 50, parent.width - 20)
        interactive: width == parent.width - 20
        orientation: "Horizontal"

        delegate: Item { 
            width: 50; height: 50 
            id: delegateRoot

            Image {
                id: image
                source: modelData.icon
                smooth: true
                scale: 0.8
                transformOrigin: "Center"
            }

            MouseArea {
                anchors.fill: parent
                onClicked: { root.currentIndex = index }
            }

            states: State { 
                name: "Selected"
                when: delegateRoot.ListView.isCurrentItem == true
                PropertyChanges { 
                    target: image
                    scale: 1
                    y: -5
                }
            }
            transitions: Transition {
                NumberAnimation {
                    properties: "scale,y"
                }
             }
        }
        model: visualModel.children
    }
}
