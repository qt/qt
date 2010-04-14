import Qt 4.7
import "content"
import "content/tic-tac-toe.js" as Logic

Item {
    id: game

    property bool show: false
    property real difficulty: 1.0   //chance it will actually think

    width: 440
    height: 480
    anchors.fill: parent

    Image {
        id: boardimage
        anchors { verticalCenter: parent.verticalCenter; horizontalCenter: parent.horizontalCenter }
        source: "content/pics/board.png"
    }

    Grid {
        id: board
        anchors.fill: boardimage
        columns: 3

        Repeater {
            model: 9
            TicTac {
                width: board.width/3
                height: board.height/3
                onClicked: {
                    if (!endtimer.running) {
                        if (!Logic.makeMove(index,"X"))
                            Logic.computerTurn()
                    }
                }
            }
        }

        Timer {
            id: endtimer
            interval: 1600
            onTriggered: { msg.opacity = 0; Logic.restart() }
        }
    }

    Row {
        spacing: 4
        anchors { top: board.bottom; horizontalCenter: board.horizontalCenter }

        Button {
            text: "Hard"
            onClicked: game.difficulty = 1.0;
            down: game.difficulty == 1.0
        }
        Button {
            text: "Moderate"
            onClicked: game.difficulty = 0.8;
            down: game.difficulty == 0.8
        }
        Button {
            text: "Easy"
            onClicked: game.difficulty = 0.2;
            down: game.difficulty == 0.2
        }
    }

    Text {
        id: msg

        anchors.centerIn: parent
        opacity: 0
        color: "blue"
        style: Text.Outline; styleColor: "white"
        font.pixelSize: 50; font.bold: true
    }
}
