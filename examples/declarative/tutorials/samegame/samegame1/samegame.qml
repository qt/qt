//![0]
import Qt 4.6

Rectangle {
    id: Screen
    width: 490; height: 720

    SystemPalette { id: activePalette; colorGroup: Qt.Active }

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

        Button {
            id: btnA; text: "New Game"; onClicked: print("Implement me!");
            anchors.left: parent.left; anchors.leftMargin: 3
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            id: Score
            text: "Score: Who knows?"; font.bold: true
            anchors.right: parent.right; anchors.rightMargin: 3
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
//![0]
