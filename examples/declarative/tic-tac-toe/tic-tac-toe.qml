import Qt 4.6
import "content"
import "tic-tac-toe.js" as Logic

Item {
    id: game
    property bool show: false;
    width: 440
    height: 480
    anchors.fill: parent
    property real difficulty: 1.0; //chance it will actually think

    Image {
        id: boardimage
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
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
        anchors.top: board.bottom
        anchors.horizontalCenter: board.horizontalCenter
        Button {
            text: "Hard"
            onClicked: game.difficulty=1.0;
            down: game.difficulty == 1.0
        }
        Button {
            text: "Moderate"
            onClicked: game.difficulty=0.8;
            down: game.difficulty == 0.8
        }
        Button {
            text: "Easy"
            onClicked: game.difficulty=0.2;
            down: game.difficulty == 0.2
        }
    }

    Text {
        id: msg
        opacity: 0
        anchors.centerIn: parent
        color: "blue"
        styleColor: "white"
        style: Text.Outline
        font.pixelSize: 50
        font.bold: true
    }
}
