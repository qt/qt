import Qt 4.6
import "content"

Item {
    width: boardimage.width
    height: boardimage.height

    Image {
        id: boardimage
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
                        if (!makeMove(index,"X"))
                            computerTurn()
                    }
                }
            }
        }

        Script {
            function winner()
            {
                for (var i=0; i<3; ++i) {
                    if (board.children[i].state!=""
                        && board.children[i].state==board.children[i+3].state
                        && board.children[i].state==board.children[i+6].state)
                        return true

                    if (board.children[i*3].state!=""
                        && board.children[i*3].state==board.children[i*3+1].state
                        && board.children[i*3].state==board.children[i*3+2].state)
                        return true
                }

                if (board.children[0].state!=""
                    && board.children[0].state==board.children[4].state!=""
                    && board.children[0].state==board.children[8].state!="")
                    return true

                if (board.children[2].state!=""
                    && board.children[2].state==board.children[4].state!=""
                    && board.children[2].state==board.children[6].state!="")
                    return true

                return false
            }

            function restart()
            {
                // No moves left - start again
                for (var i=0; i<9; ++i)
                    board.children[i].state = ""
            }

            function makeMove(pos,player)
            {
                board.children[pos].state = player
                if (winner()) {
                    win(player + " wins")
                    return true
                } else {
                    return false
                }
            }

            function computerTurn()
            {
                // world's dumbest player
                for (var i=0; i<9; ++i)
                    if (board.children[i].state == "") {
                        makeMove(i,"O")
                        return
                    }
                restart()
            }

            function win(s)
            {
                msg.text = s
                msg.opacity = 1
                endtimer.running = true
            }
        }

        Timer {
            id: endtimer
            interval: 1600
            onTriggered: { msg.opacity = 0; restart() }
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
