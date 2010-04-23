import Qt 4.7

Component {
    Item {
        id: container
        x: 5; width: ListView.view.width - 10; height: 60

        Rectangle {
            id: content
            anchors.centerIn: parent; width: container.width - 40; height: container.height - 10
            color: "transparent"
            smooth: true
            radius: 10

            Rectangle { color: "#91AA9D"; x: 3; y: 3; width: parent.width - 6; height: parent.height - 6; radius: 8 }
            Text {
                text: "List element " + (index + 1); color: "#193441"; font.bold: false; anchors.centerIn: parent
                font.pixelSize: 14
            }
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true

            onClicked: {
                ListView.view.currentIndex = index
                container.focus = true
                ListView.view.focus = true
                listViews.focus = true
                mainView.focus = true
            }
        }

        states: State {
            name: "active"; when: container.focus == true
            PropertyChanges { target: content; color: "#FCFFF5"; scale: 1.1 }
        }

        transitions: Transition {
            NumberAnimation { properties: "scale"; duration: 100 }
        }
    }
}
