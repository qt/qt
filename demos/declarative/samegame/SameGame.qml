import "content"

Rect {
    width: 400
    height: 700
    color: "white"
    Script { source: "content/samegame.js" }
    Rect{
        property int score: 0
        x:20; y:20; width:360; height:600; id: gameCanvas;
        color: "white"
        pen.width: 1
        Image { id:background;
            source: "content/pics/qtlogo.png"
            anchors.fill: parent
        }

        MouseRegion { id: gameMR; anchors.fill: parent;
            onClicked: handleClick(mouseX, mouseY);
        }
    }
    HorizontalLayout {
        anchors.top: gameCanvas.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        MediaButton { id: btnA; text: "New Game"; onClicked: {initBoard();} }
        MediaButton { id: btnB; text: "Swap Theme"; onClicked: {swapTileSrc(); dialog.opacity = 1;}
        }
        Text{ text: "Score: " + gameCanvas.score; width:100 }
    }
    SameDialog {
        id: dialog
        anchors.centeredIn: parent
        text: "Takes effect next game."
    }
}
