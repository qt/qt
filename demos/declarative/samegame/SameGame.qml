import Qt 4.6

import "content"

Rect {
    id: page; width: 460; height: 700; color: activePalette.window
    Script { source: "content/samegame.js" }
    Palette { id: activePalette; colorGroup: "Active" }
    Rect {
        id: gameCanvas
        property int score: 0
        z:20; y:20; width:400; height:600; color: "white"; pen.width: 1
        anchors.horizontalCenter: parent.horizontalCenter
        Image { id:background;
            source: "content/pics/background.png"
            anchors.fill: parent
        }
        MouseRegion { id: gameMR
            anchors.fill: parent; onClicked: handleClick(mouse.x,mouse.y);
        }
    }

    Dialog { id: dialog; anchors.centeredIn: parent; z: 21}
    Button { 
        id: btnA; text: "New Game"; onClicked: {initBoard();} 
        anchors.top: gameCanvas.bottom; anchors.topMargin: 4; anchors.left: gameCanvas.left;
    }
    Text { 
        text: "Score: " + gameCanvas.score; width:100; font.size:14 
        anchors.top: gameCanvas.bottom; anchors.topMargin: 4; anchors.right: gameCanvas.right;
    }
    Text { 
        text: "Just over 300 lines of QML/JS code!"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom; anchors.bottomMargin: 16;
        opacity: SequentialAnimation{ running: true; repeat: true;
            NumberAnimation { from: 0; to: 1; duration: 1000; easing: "easeInQuad" }
            NumberAnimation { from: 1; to: 0; duration: 1000; easing: "easeInQuad" }
        }
    }
}
