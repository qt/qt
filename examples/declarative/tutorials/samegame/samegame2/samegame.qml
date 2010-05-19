import Qt 4.7
//![2]
import "samegame.js" as SameGame
//![2]

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
    }

    Rectangle {
        id: toolBar
        width: parent.width; height: 32
        color: activePalette.window
        anchors.bottom: screen.bottom

//![1]
        Button {
            anchors { left: parent.left; verticalCenter: parent.verticalCenter }
            text: "New Game"
            onClicked: SameGame.startNewGame()
        }
//![1]

        Text {
            id: score
            anchors { right: parent.right; verticalCenter: parent.verticalCenter }
            text: "Score: Who knows?"
        }
    }
}
