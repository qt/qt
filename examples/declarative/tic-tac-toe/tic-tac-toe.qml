import Qt 4.6
import "content"

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
                        if (!makeMove(index,"X"))
                            computerTurn()
                    }
                }
            }
        }

        Script {
            function winner(board)
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
                if (winner(board)) {
                    win(player + " wins")
                    return true
                } else {
                    return false
                }
            }

            function computerTurn()
            {
                var r = Math.random();
                if(r < game.difficulty){
                    smartAI();
                }else{
                    randAI();
                }
            }

            function smartAI()
            {
                function boardCopy(a){
                    var ret = new Object;
                    ret.children = new Array(9);
                    for(var i = 0; i<9; i++){
                        ret.children[i] = new Object;
                        ret.children[i].state = a.children[i].state;
                    }
                    return ret;
                }
                for(var i=0; i<9; i++){
                    var simpleBoard = boardCopy(board);
                    if (board.children[i].state == "") {
                        simpleBoard.children[i].state = "O";
                        if(winner(simpleBoard)){
                            makeMove(i,"O")
                            return
                        }
                    }
                }
                for(var i=0; i<9; i++){
                    var simpleBoard = boardCopy(board);
                    if (board.children[i].state == "") {
                        simpleBoard.children[i].state = "X";
                        if(winner(simpleBoard)){
                            makeMove(i,"O")
                            return
                        }
                    }
                }
                function thwart(a,b,c){//If they are at a, try b or c
                    if (board.children[a].state == "X") {
                        if (board.children[b].state == "") {
                            makeMove(b,"O")
                            return true
                        }else if (board.children[c].state == "") {
                            makeMove(c,"O")
                            return true
                        }
                    }
                    return false;
                }
                if(thwart(4,0,2)) return;
                if(thwart(0,4,3)) return;
                if(thwart(2,4,1)) return;
                if(thwart(6,4,7)) return;
                if(thwart(8,4,5)) return;
                if(thwart(1,4,2)) return;
                if(thwart(3,4,0)) return;
                if(thwart(5,4,8)) return;
                if(thwart(7,4,6)) return;
                for(var i =0; i<9; i++){//Backup
                    if (board.children[i].state == "") {
                        makeMove(i,"O")
                        return
                    }
                }
                restart();
            }

            function randAI()
            {
                var open = 0;
                for (var i=0; i<9; ++i)
                    if (board.children[i].state == "") {
                        open += 1;
                    }
                if(open == 0){
                    restart();
                    return;
                }
                var openA = new Array(open);//JS doesn't have lists I can append to (i think)
                var acc = 0;
                for (var i=0; i<9; ++i)
                    if (board.children[i].state == "") {
                        openA[acc] = i;
                        acc += 1;
                    }
                var choice = openA[Math.floor(Math.random() * open)];
                makeMove(choice, "O");
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
