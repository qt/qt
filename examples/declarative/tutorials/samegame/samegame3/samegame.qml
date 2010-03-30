//![0]
import Qt 4.6

Rectangle {
    id: screen
    width: 490; height: 720

    SystemPalette { id: activePalette }
    Script { source: "samegame.js" }

    Item {
        width: parent.width; anchors.top: parent.top; anchors.bottom: toolbar.top

        Image {
            id: background
            anchors.fill: parent; source: "../shared/pics/background.jpg"
            fillMode: Image.PreserveAspectCrop
        }

//![1]
        Item {
            id: gameCanvas
            property int score: 0
            property int tileSize: 40

            z: 20; anchors.centerIn: parent
            width: parent.width - (parent.width % tileSize);
            height: parent.height - (parent.height % tileSize);

            MouseArea {
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
        id: toolbar
        color: activePalette.window
        height: 32; width: parent.width
        anchors.bottom: screen.bottom

        Button {
            id: btnA; text: "New Game"; onClicked: initBoard();
            anchors.left: parent.left; anchors.leftMargin: 3
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            id: score
            text: "Score: " + gameCanvas.score; font.bold: true
            anchors.right: parent.right; anchors.rightMargin: 3
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
//![0]
