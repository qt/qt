import Qt 4.6
import SamegameCore 1.0
import "SamegameCore/samegame.js" as Logic

Rectangle {
    id: screen
    width: 490; height: 720

    SystemPalette { id: activePalette }

    Item {
        width: parent.width; anchors.top: parent.top; anchors.bottom: toolBar.top

        Image {
            id: background
            anchors.fill: parent; source: "SamegameCore/pics/background.png"
            fillMode: Image.PreserveAspectCrop
            smooth: true
        }

        Item {
            id: gameCanvas
            property int score: 0
            property int tileSize: 40

            z: 20; anchors.centerIn: parent
            width: parent.width - (parent.width % tileSize);
            height: parent.height - (parent.height % tileSize);

            MouseArea {
                id: gameMR
                anchors.fill: parent; onClicked: Logic.handleClick(mouse.x,mouse.y);
            }
        }
    }

    Dialog { id: dialog; anchors.centerIn: parent; z: 21 }
    Dialog {
        id: scoreName; anchors.centerIn: parent; z: 22;
        property int initialWidth: 0
        Behavior on width {NumberAnimation{} enabled: initialWidth!=0}
        Text {
            id: spacer
            anchors.left: scoreName.left
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            text: "You won! Please enter your name: "
        }
        TextInput {
            id: editor
            onTextChanged: {
                var newWidth = editor.width + spacer.width + 40;
                if((newWidth > scoreName.width && newWidth < screen.width) 
                        || (scoreName.width > scoreName.initialWidth))
                    scoreName.width = newWidth;
            }
            onAccepted: {
                if(scoreName.opacity==1&&editor.text!="")
                    Logic.saveHighScore(editor.text);
                scoreName.forceClose();
            }
            anchors.verticalCenter: parent.verticalCenter
            focus: true
            anchors.left: spacer.right
        }
    }

    Rectangle {
        id: toolBar
        color: activePalette.window
        height: 32; width: parent.width
        anchors.bottom: screen.bottom

        Button {
            id: btnA; text: "New Game"; onClicked: Logic.initBoard();
            anchors.left: parent.left; anchors.leftMargin: 3
            anchors.verticalCenter: parent.verticalCenter
        }

        Button {
            id: btnB; text: "Quit"; onClicked: Qt.quit();
            anchors.left: btnA.right; anchors.leftMargin: 3
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            id: score
            text: "Score: " + gameCanvas.score; font.bold: true
            anchors.right: parent.right; anchors.rightMargin: 3
            anchors.verticalCenter: parent.verticalCenter
            color: activePalette.windowText
        }
    }
}
