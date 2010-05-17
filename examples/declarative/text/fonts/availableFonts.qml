import Qt 4.7

Rectangle {
    width: 480; height: 640; color: "steelblue"

    ListView {
        anchors.fill: parent; model: Qt.fontFamilies()

        delegate: Item {
            height: 40; width: ListView.view.width
            Text {
                anchors.centerIn: parent
                text: modelData; font.family: modelData; font.pixelSize: 24; color: "white"
            }
        }
    }
}
