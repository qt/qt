import Qt 4.6
import "content"

Rectangle {
    id: screen;
    SystemPalette { id: activePalette }
    color: activePalette.window

    Script { source: "content/snake.js" }

    property int gridSize : 34
    property int margin: 4
    property int numRowsAvailable: Math.floor((height-32-2*margin)/gridSize)
    property int numColumnsAvailable: Math.floor((width-2*margin)/gridSize)

    property int lastScore : 0

    property int score: 0;
    property int heartbeatInterval: 200
    property int halfbeatInterval: 160

    width: 480
    height: 750

    property int direction
    property int headDirection

    property var head;

    HighScoreModel {
        id: highScores
        game: "Snake"
    }

    Timer {
        id: heartbeat;
        interval: heartbeatInterval;
        repeat: true
        onTriggered: { move() }
    }
    Timer {
        id: halfbeat;
        interval: halfbeatInterval;
        repeat: true
        running: heartbeat.running
        onTriggered: { moveSkull() }
    }
    Timer {
        id: startNewGameTimer;
        interval: 700;
        onTriggered: {startNewGame(); }
    }

    Timer {
        id: startHeartbeatTimer;
        interval: 1000 ;
    }


    Image {
        Image {
            id: title
            source: "content/pics/snake.jpg"
            fillMode: "PreserveAspectCrop"
            anchors.fill: parent
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            Behavior on opacity { NumberAnimation { duration: 500 } }

            Text {
                color: "white"
                font.pointSize: 24
                horizontalAlignment: "AlignHCenter"
                text: "Last Score:\t" + lastScore + "\nHighscore:\t" + highScores.topScore;
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: gridSize
            }
        }

        source: "content/pics/background.png"
        fillMode: "PreserveAspectCrop"

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: toolbar.top

        Rectangle {
            id: playfield
            border.width: 1
            border.color: "white"
            color: "transparent"
            anchors.horizontalCenter: parent.horizontalCenter
            y: (screen.height - 32 - height)/2;
            width: numColumnsAvailable * gridSize + 2*margin
            height: numRowsAvailable * gridSize + 2*margin


            Skull {
                id: skull
            }

            MouseArea {
                anchors.fill: parent
                onPressed: {
                    if (!head || !heartbeat.running) {
                        startNewGame();
                        return;
                    }
                    if (direction == 0 || direction == 2)
                        scheduleDirection((mouseX > (head.x + head.width/2)) ? 1 : 3);
                    else
                        scheduleDirection((mouseY > (head.y + head.height/2)) ? 2 : 0);
                }
            }
        }

    }

    Rectangle {
        id: progressBar
        opacity: 0
        Behavior on opacity { NumberAnimation { duration: 200 } }
        color: "transparent"
        border.width: 2
        border.color: "#221edd"
        x: 50
        y: 50
        width: 200
        height: 30
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: 40

        Rectangle {
            id: progressIndicator
            color: "#221edd";
            width: 0;
            Behavior on width { NumberAnimation { duration: startHeartbeatTimer.running ? 1000 : 0}}
            height: 30;
        }
    }

    Rectangle {
        id: toolbar
        color: activePalette.window
        height: 32; width: parent.width
        anchors.bottom: screen.bottom

        Button {
            id: btnA; text: "New Game"; onClicked: {startNewGame();}
            anchors.left: parent.left; anchors.leftMargin: 3
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            color: activePalette.text
            text: "Score: " + score; font.bold: true
            anchors.right: parent.right; anchors.rightMargin: 3
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    focus: true
    Keys.onSpacePressed: startNewGame();
    Keys.onLeftPressed: if (state == "starting" || direction != 1) scheduleDirection(3);
    Keys.onRightPressed: if (state == "starting" || direction != 3) scheduleDirection(1);
    Keys.onUpPressed: if (state == "starting" || direction != 2) scheduleDirection(0);
    Keys.onDownPressed: if (state == "starting" || direction != 0) scheduleDirection(2);

    states: [
        State {
            name: "starting"
            when: startHeartbeatTimer.running
            PropertyChanges {target: progressIndicator; width: 200}
            PropertyChanges {target: title; opacity: 0}
            PropertyChanges {target: progressBar; opacity: 1}
        },
        State {
            name: "running"
            when: (heartbeat.running && !startHeartbeatTimer.running)
            PropertyChanges {target: progressIndicator; width: 200}
            PropertyChanges {target: title; opacity: 0}
            PropertyChanges {target: skull; row: 0; column: 0; }
            PropertyChanges {target: skull; spawned: 1}
        }
    ]

}
