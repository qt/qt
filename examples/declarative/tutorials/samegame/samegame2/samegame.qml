import Qt 4.6

Rectangle {
    id: Screen
    width: 490; height: 720

    SystemPalette { id: activePalette; colorGroup: Qt.Active }
//![2]
    Script { source: "samegame.js" }
//![2]

    Item {
        width: parent.width; anchors.top: parent.top; anchors.bottom: ToolBar.top

        Image {
            id: background
            anchors.fill: parent; source: "pics/background.png"
            fillMode: Image.PreserveAspectCrop
        }
    }

    Rectangle {
        id: ToolBar
        color: activePalette.window
        height: 32; width: parent.width
        anchors.bottom: Screen.bottom

//![1]
        Button {
            id: btnA; text: "New Game"; onClicked: initBoard();
            anchors.left: parent.left; anchors.leftMargin: 3
            anchors.verticalCenter: parent.verticalCenter
        }
//![1]

        Text {
            id: Score
            text: "Score: Who knows?"; font.bold: true
            anchors.right: parent.right; anchors.rightMargin: 3
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
