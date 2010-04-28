//![0]
import Qt 4.7
import "samegame.js" as SameGame

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

//![1]
        Item {
            id: gameCanvas

            property int score: 0
            property int blockSize: 40

            width: parent.width - (parent.width % blockSize)
            height: parent.height - (parent.height % blockSize)
            anchors.centerIn: parent
            z: 20

            MouseArea {
                anchors.fill: parent
                onClicked: SameGame.handleClick(mouse.x, mouse.y)
            }
        }
//![1]
    }

//![2]
    Dialog { id: dialog; anchors.centerIn: parent; z: 21 }
//![2]

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
            text: "Score: Who knows?"
            font.bold: true
        }
    }
}
//![0]
