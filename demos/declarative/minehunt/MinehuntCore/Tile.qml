import Qt 4.7

Flipable {
    id: flipable
    property int angle: 0

    width: 40;  height: 40
    transform: Rotation { origin.x: 20; origin.y: 20; axis.x: 1; axis.z: 0; angle: flipable.angle }

    front: Image {
        source: "pics/front.png"; width: 40; height: 40

        Image {
            anchors.centerIn: parent
            source: "pics/flag.png"; opacity: modelData.hasFlag

            Behavior on opacity { NumberAnimation {} }
        }
    }

    back: Image {
        source: "pics/back.png"
        width: 40; height: 40

        Text {
            anchors.centerIn: parent
            text: modelData.hint; color: "white"; font.bold: true
            opacity: !modelData.hasMine && modelData.hint > 0
        }

        Image {
            anchors.centerIn: parent
            source: "pics/bomb.png"; opacity: modelData.hasMine
        }

        Explosion { id: expl }
    }

    states: State {
        name: "back"; when: modelData.flipped
        PropertyChanges { target: flipable; angle: 180 }
    }

    property real pauseDur: 250
    transitions: Transition {
        SequentialAnimation {
            ScriptAction {
                script: {
                    var ret = Math.abs(flipable.x - field.clickx)
                        + Math.abs(flipable.y - field.clicky);
                    if (modelData.hasMine && modelData.flipped)
                        pauseDur = ret * 3
                    else
                        pauseDur = ret
                }
            }
            PauseAnimation {
                duration: pauseDur
            }
            RotationAnimation { easing.type: "InOutQuad" }
            ScriptAction { script: if (modelData.hasMine && modelData.flipped) { expl.explode = true } }
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            field.clickx = flipable.x
            field.clicky = flipable.y
            var row = Math.floor(index / 9)
            var col = index - (Math.floor(index / 9) * 9)
            if (mouse.button == undefined || mouse.button == Qt.RightButton) {
                flag(row, col)
            } else {
                flip(row, col)
            }
        }
        onPressAndHold: {
            field.clickx = flipable.x
            field.clicky = flipable.y
            var row = Math.floor(index / 9)
            var col = index - (Math.floor(index / 9) * 9)
            flag(row, col)
        }
    }
}
