import Qt 4.7
import "MinehuntCore" 1.0

Item {
    id: field
    property int clickx: 0
    property int clicky: 0

    width: 450; height: 450

    Component {
        id: tile

        Flipable {
            id: flipable
            property int angle: 0

            width: 40;  height: 40
            transform: Rotation { origin.x: 20; origin.y: 20; axis.x: 1; axis.z: 0; angle: flipable.angle }

            front: Image {
                source: "MinehuntCore/pics/front.png"; width: 40; height: 40

                Image {
                    anchors.horizontalCenter: parent.horizontalCenter; anchors.verticalCenter: parent.verticalCenter
                    source: "MinehuntCore/pics/flag.png"; opacity: modelData.hasFlag

                    Behavior on opacity { NumberAnimation { property: "opacity"; duration: 250 } }
                }
            }

            back: Image {
                source: "MinehuntCore/pics/back.png"
                width: 40; height: 40

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter; anchors.verticalCenter: parent.verticalCenter
                    text: modelData.hint; color: "white"; font.bold: true
                    opacity: !modelData.hasMine && modelData.hint > 0
                }

                Image {
                    anchors.horizontalCenter: parent.horizontalCenter; anchors.verticalCenter: parent.verticalCenter
                    source: "MinehuntCore/pics/bomb.png"; opacity: modelData.hasMine
                }

                Explosion { id: expl }
            }

            states: State {
                name: "back"; when: modelData.flipped
                PropertyChanges { target: flipable; angle: 180 }
            }

            transitions: Transition {
                SequentialAnimation {
                    PauseAnimation {
                        duration: {
                            var ret
                            if (flipable.parent != null)
                                ret = Math.abs(flipable.parent.x - field.clickx)
                                + Math.abs(flipable.parent.y - field.clicky)
                                else
                                    ret = 0
                            if (ret > 0) {
                                if (modelData.hasMine && modelData.flipped) {
                                    ret * 3
                                } else {
                                    ret
                                }
                            } else {
                                0
                            }
                        }
                    }
                    NumberAnimation { easing.type: "InOutQuad"; properties: "angle" }
                    ScriptAction { script: if (modelData.hasMine && modelData.flipped) { expl.explode = true } }
                }
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: {
                    field.clickx = flipable.parent.x
                    field.clicky = flipable.parent.y
                    var row = Math.floor(index / 9)
                    var col = index - (Math.floor(index / 9) * 9)
                    if (mouse.button == undefined || mouse.button == Qt.RightButton) {
                        flag(row, col)
                    } else {
                        flip(row, col)
                    }
                }
                onPressAndHold: {
                    field.clickx = flipable.parent.x
                    field.clicky = flipable.parent.y
                    var row = Math.floor(index / 9)
                    var col = index - (Math.floor(index / 9) * 9)
                    flag(row, col)
                }
            }
        }
    }

    Image { source: "MinehuntCore/pics/No-Ones-Laughing-3.jpg"; anchors.fill: parent; fillMode: Image.Tile }

    Grid {
        anchors.horizontalCenter: parent.horizontalCenter
        columns: 9; spacing: 1

        Repeater {
            id: repeater
            model: tiles
            Component { Loader { sourceComponent: tile } }
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
        anchors.fill: parent; wrapMode: Text.WordWrap
        text: "Minehunt will not run properly if the C++ plugin is not compiled.\nPlease see README."
        color: "white"; font.bold: true; font.pixelSize: 14
        visible: tiles == undefined
    }

}
