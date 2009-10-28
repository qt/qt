import Qt 4.6

Item {
    id: root

    property alias background: background.source
    default property alias content: visualModel.children
    property int currentIndex: 0

    Image {
        id: background
        fillMode: Image.TileHorizontally
        x: -list.viewportX / 2
        width: Math.max(list.viewportWidth, parent.width)
    }

    ListView {
        id: list

        currentIndex: root.currentIndex
        onCurrentIndexChanged: root.currentIndex = currentIndex 

        orientation: "Horizontal"
        overShoot: false
        anchors.fill: parent
        model: VisualItemModel { id: visualModel }

        highlight: Rectangle { height: 1; width: 1 }
        highlightMoveSpeed: 2000
        preferredHighlightBegin: 0
        preferredHighlightEnd: 0
        highlightRangeMode: "StrictlyEnforceRange"

        flickDeceleration: 1000
    }

    ListView {
        id: selector

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

            MouseRegion {
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

        Rectangle {
            color: "#40FFFFFF"
            x: -10; 
            y: -10;
            width: parent.width + 20; height: parent.height + 10
        }
    }
}
