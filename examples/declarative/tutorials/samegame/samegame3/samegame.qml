//![0]
import Qt 4.6

Rectangle {
    id: Screen
    width: 490; height: 720

    SystemPalette { id: activePalette; colorGroup: Qt.Active }
    Script { source: "samegame.js" }

    Item {
        width: parent.width; anchors.top: parent.top; anchors.bottom: ToolBar.top

        Image {
            id: background
            anchors.fill: parent; source: "pics/background.png"
            fillMode: "PreserveAspectCrop"
        }

        //![1]
        Item {
            id: gameCanvas
            property int score: 0
            property int tileSize: 40

            z: 20; anchors.centerIn: parent
            width: parent.width - (parent.width % tileSize);
            height: parent.height - (parent.height % tileSize);

            MouseRegion {
                id: gameMR
                anchors.fill: parent; onClicked: handleClick(mouse.x,mouse.y);
            }
        }
        //![1]
    }

    //![2]
    Dialog { id: dialog; anchors.centerIn: parent; z: 21 }
    //![2]

    Rectangle {
        id: ToolBar
        color: activePalette.window
        height: 32; width: parent.width
        anchors.bottom: Screen.bottom

        Button {
            id: btnA; text: "New Game"; onClicked: initBoard();
            anchors.left: parent.left; anchors.leftMargin: 3
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            id: Score
            text: "Score: " + gameCanvas.score; font.bold: true
            anchors.right: parent.right; anchors.rightMargin: 3
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
//![0]
