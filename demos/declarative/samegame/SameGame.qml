import Qt 4.6
import "content"

Rectangle {
    id: Screen
    width: 460; height: 700

    Script { source: "content/samegame.js" }

    SystemPalette { id: activePalette; colorGroup: Qt.Active }

    Item {
        width: parent.width; anchors.top: parent.top; anchors.bottom: ToolBar.top

        Image {
            id: background
            anchors.fill: parent; source: "content/pics/background.png"
            fillMode: "PreserveAspectCrop"
        }

        Item {
            id: gameCanvas
            property int score: 0

            z: 20; anchors.centerIn: parent
            width: parent.width - tileSize - (parent.width % tileSize);
            height: parent.height - tileSize - (parent.height % tileSize);

            MouseRegion {
                id: gameMR
                anchors.fill: parent; onClicked: handleClick(mouse.x,mouse.y);
            }
        }
    }

    Dialog { id: dialog; anchors.centerIn: parent; z: 21 }

    Rectangle {
        id: ToolBar
        color: activePalette.window
        height: 32; width: parent.width
        anchors.bottom: Screen.bottom

        Button {
            id: btnA; text: "New Game"; onClicked: {initBoard();}
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
