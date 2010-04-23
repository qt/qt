import Qt 4.7

Item {
    id: qtLogo

    property alias fillMode: image.fillMode
    property alias label: labelText.text

    width: ((window.width - 20) - ((grid.columns - 1) * grid.spacing)) / grid.columns
    height: ((window.height - 20) - ((grid.rows - 1) * grid.spacing)) / grid.rows

    Column {
        anchors.fill: parent

        Rectangle {
            height: qtLogo.height - 30; width: qtLogo.width
            border.color: "black"; clip: true

            Image {
                id: image
                anchors.fill: parent; smooth: true; source: "qt-logo.png"
            }
        }

        Text {
            id: labelText; anchors.horizontalCenter: parent.horizontalCenter
            height: 30; verticalAlignment: Text.AlignVCenter
        }
    }
}
