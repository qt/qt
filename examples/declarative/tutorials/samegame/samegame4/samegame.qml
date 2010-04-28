import Qt 4.7
import "content"
import "content/samegame.js" as SameGame

Rectangle {
    id: screen

    width: 490; height: 720

    SystemPalette { id: activePalette }

    Item {
        width: parent.width
        anchors { top: parent.top; bottom: toolBar.top }

        Image {
            id: background
            anchors.fill: parent
            source: "../shared/pics/background.jpg"
            fillMode: Image.PreserveAspectCrop
        }

        Item {
            id: gameCanvas
            property int score: 0
            property int blockSize: 40

            z: 20; anchors.centerIn: parent
            width: parent.width - (parent.width % blockSize);
            height: parent.height - (parent.height % blockSize);

            MouseArea {
                anchors.fill: parent; onClicked: SameGame.handleClick(mouse.x,mouse.y);
            }
        }
    }

    Dialog { id: dialog; anchors.centerIn: parent; z: 21 }

    //![0]
    Dialog {
        id: nameInputDialog

        anchors.centerIn: parent
        z: 22

        Text {
            id: dialogText
            opacity: 0
            text: "   You won! Please enter your name:"
        }

        TextInput {
            id: nameInput
            width: 72
            anchors { verticalCenter: parent.verticalCenter; left: dialogText.right }
            focus: true

            onAccepted: {
                if (nameInputDialog.opacity == 1 && nameInput.text != "")
                    SameGame.saveHighScore(nameInput.text);
                nameInputDialog.forceClose();
            }
        }
    }
    //![0]

    Rectangle {
        id: toolBar
        width: parent.width; height: 32
        color: activePalette.window
        anchors.bottom: screen.bottom

        Button {
            anchors { left: parent.left; leftMargin: 3; verticalCenter: parent.verticalCenter }
            text: "New Game"
            onClicked: SameGame.startNewGame()
        }

        Text {
            id: score
            anchors { right: parent.right; rightMargin: 3; verticalCenter: parent.verticalCenter }
            text: "Score: " + gameCanvas.score
            font.bold: true
        }
    }
}
