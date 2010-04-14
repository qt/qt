import Qt 4.7
import "MinehuntCore" 1.0

Item {
    id: field
    property int clickx: 0
    property int clicky: 0

    width: 450; height: 450

    Image { source: "MinehuntCore/pics/No-Ones-Laughing-3.jpg"; anchors.fill: parent; fillMode: Image.Tile }

    Grid {
        anchors.horizontalCenter: parent.horizontalCenter
        columns: 9; spacing: 1

        Repeater {
            id: repeater
            model: tiles
            delegate: Tile {}
        }
    }

    Row {
        id: gamedata
        x: 20; spacing: 20
        anchors.bottom: field.bottom; anchors.bottomMargin: 15

        Column {
            spacing: 2
            Image { source: "MinehuntCore/pics/bomb-color.png" }
            Text { anchors.horizontalCenter: parent.horizontalCenter; color: "white"; text: numMines }
        }

        Column {
            spacing: 2
            Image { source: "MinehuntCore/pics/flag-color.png" }
            Text { anchors.horizontalCenter: parent.horizontalCenter; color: "white"; text: numFlags }
        }
    }

    Image {
        anchors.bottom: field.bottom; anchors.bottomMargin: 15
        anchors.right: field.right; anchors.rightMargin: 20
        source: isPlaying ? 'MinehuntCore/pics/face-smile.png' :
        hasWon ? 'MinehuntCore/pics/face-smile-big.png': 'MinehuntCore/pics/face-sad.png'

        MouseArea { anchors.fill: parent; onPressed: reset() }
    }
    Text {
        anchors.centerIn: parent; width: parent.width - 20
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        text: "Minehunt will not run properly if the C++ plugin is not compiled.\n\nPlease see README."
        color: "white"; font.bold: true; font.pixelSize: 14
        visible: tiles == undefined
    }

}
