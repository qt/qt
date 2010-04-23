import Qt 4.7
import "SamegameCore"
import "SamegameCore/samegame.js" as Logic

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
            source: "SamegameCore/pics/background.png"
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
                anchors.fill: parent; onClicked: Logic.handleClick(mouse.x,mouse.y);
            }
        }
    }

    Dialog { id: dialog; anchors.centerIn: parent; z: 21 }

    Dialog {
        id: nameInputDialog

        property int initialWidth: 0

        anchors.centerIn: parent
        z: 22;

        Behavior on width {
            NumberAnimation {} 
            enabled: initialWidth != 0
        }

        Text {
            id: dialogText
            anchors { left: nameInputDialog.left; leftMargin: 20; verticalCenter: parent.verticalCenter }
            text: "You won! Please enter your name: "
        }

        TextInput {
            id: nameInputText
            anchors { verticalCenter: parent.verticalCenter; left: dialogText.right }
            focus: true

            onTextChanged: {
                var newWidth = nameInputText.width + dialogText.width + 40;
                if ( (newWidth > nameInputDialog.width && newWidth < screen.width) 
                        || (nameInputDialog.width > nameInputDialog.initialWidth) )
                    nameInputDialog.width = newWidth;
            }
            onAccepted: {
                if (nameInputDialog.opacity == 1 && nameInputText.text != "")
                    Logic.saveHighScore(nameInputText.text);
                nameInputDialog.forceClose();
            }
        }
    }

    Rectangle {
        id: toolBar
        width: parent.width; height: 32
        color: activePalette.window
        anchors.bottom: screen.bottom

        Button {
            id: newGameButton
            anchors { left: parent.left; leftMargin: 3; verticalCenter: parent.verticalCenter }
            text: "New Game" 
            onClicked: Logic.startNewGame()
        }

        Button {
            text: "Quit"
            anchors { left: newGameButton.right; leftMargin: 3; verticalCenter: parent.verticalCenter }
            onClicked: Qt.quit();
        }

        Text {
            id: score
            anchors { right: parent.right; rightMargin: 3; verticalCenter: parent.verticalCenter }
            text: "Score: " + gameCanvas.score
            font.bold: true
            color: activePalette.windowText
        }
    }
}
