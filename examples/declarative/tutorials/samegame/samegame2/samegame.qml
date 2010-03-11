import Qt 4.6

Rectangle {
    id: screen
    width: 490; height: 720

    SystemPalette { id: activePalette }
//![2]
    Script { source: "samegame.js" }
//![2]

    Item {
        width: parent.width; anchors.top: parent.top; anchors.bottom: toolbar.top

        Image {
            id: background
            anchors.fill: parent; source: "../shared/pics/background.jpg"
            fillMode: Image.PreserveAspectCrop
        }
    }

    Rectangle {
        id: toolbar
        color: activePalette.window
        height: 32; width: parent.width
        anchors.bottom: screen.bottom

//![1]
        Button {
            id: btnA; text: "New Game"; onClicked: initBoard();
            anchors.left: parent.left; anchors.leftMargin: 3
            anchors.verticalCenter: parent.verticalCenter
        }
//![1]

        Text {
            id: score
            text: "Score: Who knows?"; font.bold: true
            anchors.right: parent.right; anchors.rightMargin: 3
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
