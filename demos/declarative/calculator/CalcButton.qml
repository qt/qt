import Qt 4.6

Rectangle {
    property alias operation: label.text
    property bool toggable: false
    property bool toggled: false
    signal clicked

    id: button; width: 50; height: 30
    border.color: palette.mid; radius: 6
    gradient: Gradient {
        GradientStop { id: gradientStop1; position: 0.0; color: Qt.lighter(palette.button) }
        GradientStop { id: gradientStop2; position: 1.0; color: palette.button }
    }

    Text { id: label; anchors.centerIn: parent; color: palette.buttonText }

    MouseRegion {
        id: clickRegion
        anchors.fill: parent
        onClicked: {
            doOp(operation);
            button.clicked();
            if (!button.toggable) return;
            button.toggled ? button.toggled = false : button.toggled = true
        }
    }

    states: [
        State {
            name: "Pressed"; when: clickRegion.pressed == true
            PropertyChanges { target: gradientStop1; color: palette.dark }
            PropertyChanges { target: gradientStop2; color: palette.button }
        },
        State {
            name: "Toggled"; when: button.toggled == true
            PropertyChanges { target: gradientStop1; color: palette.dark }
            PropertyChanges { target: gradientStop2; color: palette.button }
        }
    ]
}
