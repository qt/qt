import Qt 4.6
import "content"

Rectangle {
    id: screen
    width: 490; height: 720

    Script { source: "content/samegame.js" }

    SystemPalette { id: activePalette; colorGroup: Qt.Active }

    Item {
        width: parent.width; anchors.top: parent.top; anchors.bottom: toolBar.top

        Image {
            id: background
            anchors.fill: parent; source: "content/pics/background.png"
            fillMode: "PreserveAspectCrop"
        }

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
    }

    Dialog { id: dialog; anchors.centerIn: parent; z: 21 }
    Dialog {
        id: scoreName; anchors.centerIn: parent; z: 22;
        TextInput {
            id: editor
            onAccepted: {
                if(scoreName.opacity==1&&editor.text!="")
                    sendHighScore(editor.text);
                scoreName.forceClose();
            }
            anchors.verticalCenter: parent.verticalCenter
            width: 72; focus: true
            anchors.right: scoreName.right
        }
    }

    Rectangle {
        id: toolBar
        color: activePalette.window
        height: 32; width: parent.width
        anchors.bottom: screen.bottom

        Button {
            id: btnA; text: "New Game"; onClicked: {initBoard();}
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
