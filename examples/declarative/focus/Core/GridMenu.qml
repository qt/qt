import Qt 4.7

FocusScope {
    property alias interactive: gridView.interactive

    onWantsFocusChanged: if (wantsFocus) mainView.state = ""

    Rectangle {
        anchors.fill: parent
        clip: true
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#193441" }
            GradientStop { position: 1.0; color: Qt.darker("#193441") }
        }

        GridView {
            id: gridView
            x: 20; width: parent.width - 40; height: parent.height
            cellWidth: 152; cellHeight: 152
            focus: true
            model: 12
            KeyNavigation.down: listViews
            KeyNavigation.left: contextMenu

            delegate: Item {
                id: container
                width: GridView.view.cellWidth; height: GridView.view.cellHeight

                Rectangle {
                    id: content
                    color: "transparent"
                    smooth: true
                    anchors.centerIn: parent; width: container.width - 40; height: container.height - 40; radius: 10

                    Rectangle { color: "#91AA9D"; x: 3; y: 3; width: parent.width - 6; height: parent.height - 6; radius: 8 }
                    Image { source: "images/qt-logo.png"; anchors.centerIn: parent; smooth: true }
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true

                    onClicked: {
                        GridView.view.currentIndex = index
                        container.focus = true
                        gridMenu.focus = true
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
    }
}
