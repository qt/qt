import QtQuick 2.0
import Qt.labs.inputcontext 1.0 as InputContext


Rectangle {
    id: keyboard

    radius: 5
    height: 122
    width: 324

    property bool shift: false

    gradient: Gradient {
        GradientStop { position: 0.0; color: "lightgrey" }
        GradientStop { position: 1.0; color: "white" }
    }

    Column {
        anchors.left: parent.left; anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter

        spacing: 2

        Row {
            spacing: 2
            anchors.horizontalCenter: parent.horizontalCenter

            Key { key: Qt.Key_Q; text: "q"; }
            Key { key: Qt.Key_W; text: "w"; }
            Key { key: Qt.Key_E; text: "e"; }
            Key { key: Qt.Key_R; text: "r"; }
            Key { key: Qt.Key_T; text: "t"; }
            Key { key: Qt.Key_Y; text: "y"; }
            Key { key: Qt.Key_U; text: "u"; }
            Key { key: Qt.Key_I; text: "i"; }
            Key { key: Qt.Key_O; text: "o"; }
            Key { key: Qt.Key_P; text: "p"; }
        }
        Row {
            spacing: 2
            anchors.horizontalCenter: parent.horizontalCenter

            Key { key: Qt.Key_A; text: "a"; }
            Key { key: Qt.Key_S; text: "s"; }
            Key { key: Qt.Key_D; text: "d"; }
            Key { key: Qt.Key_F; text: "f"; }
            Key { key: Qt.Key_G; text: "g"; }
            Key { key: Qt.Key_H; text: "h"; }
            Key { key: Qt.Key_J; text: "j"; }
            Key { key: Qt.Key_K; text: "k"; }
            Key { key: Qt.Key_L; text: "l"; }
        }
        Row {
            spacing: 2
            anchors.horizontalCenter: parent.horizontalCenter

            Key { key: Qt.Key_Shift; displayText: "shift"; width: 50 }
            Key { key: Qt.Key_Z; text: "z"; }
            Key { key: Qt.Key_X; text: "x"; }
            Key { key: Qt.Key_C; text: "c"; }
            Key { key: Qt.Key_V; text: "v"; }
            Key { key: Qt.Key_B; text: "b"; }
            Key { key: Qt.Key_N; text: "n"; }
            Key { key: Qt.Key_M; text: "m"; }
            Key { key: Qt.Key_Comma; text: ","; }
            Key { key: Qt.Key_Period; text: "."; }
        }

        Row {
            spacing: 2
            anchors.horizontalCenter: parent.horizontalCenter

            Key { key: Qt.Key_Enter; text: "\n"; displayText: "enter"; width: 90 }
            Key { key: Qt.Key_Space; text: " "; displayText: "space"; width: 138}
            Key { key: Qt.Key_Backspace; displayText: "backspace"; width: 90 }
        }
    }

    function keyPress(key, text)
    {
        if (key == Qt.Key_Shift)
            keyboard.shift = !keyboard.shift
        else if (keyboard.shift)
            InputContext.sendKeyPress(key, text.toUpperCase(), Qt.ShiftModifier)
        else
            InputContext.sendKeyPress(key, text)
    }

    function keyRelease(key, text)
    {
        if (key != Qt.Key_Shift) {
            if (keyboard.shift) {
                InputContext.sendKeyRelease(key, text.toUpperCase(), Qt.ShiftModifier)
                keyboard.shift = false
            } else {
                InputContext.sendKeyRelease(key, text)
            }
        }
    }
}
