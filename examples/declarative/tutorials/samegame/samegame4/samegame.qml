import Qt 4.6
import "content"

Rectangle {
    id: screen
    width: 490; height: 720

    SystemPalette { id: activePalette }

    Item {
        width: parent.width; anchors.top: parent.top; anchors.bottom: toolBar.top

        Image {
            id: background
            anchors.fill: parent; source: "../shared/pics/background.jpg"
            fillMode: Image.PreserveAspectCrop
        }

        Item {
            id: gameCanvas
            property int score: 0
            property int tileSize: 40

            Script { source: "content/samegame.js" }

            z: 20; anchors.centerIn: parent
            width: parent.width - (parent.width % getTileSize());
            height: parent.height - (parent.height % getTileSize());

            MouseArea {
                id: gameMR
                anchors.fill: parent; onClicked: handleClick(mouse.x,mouse.y);
            }
        }
    }

    Dialog { id: dialog; anchors.centerIn: parent; z: 21 }
    Dialog {
        id: scoreName; anchors.centerIn: parent; z: 22;
        Text {
            id: spacer
            opacity: 0
            text: "   You won! Please enter your name:"
        }
        TextInput {
            id: editor
            onAccepted: {
                if(scoreName.opacity==1&&editor.text!="")
                    saveHighScore(editor.text);
                scoreName.forceClose();
            }
            anchors.verticalCenter: parent.verticalCenter
            width: 72; focus: true
            anchors.left: spacer.right
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
