import Qt 4.6

Rectangle {
    property string operation
    property bool toggable : false
    property bool toggled : false
    signal clicked

    id: Button; width: 50; height: 30
    border.color: Palette.mid; radius: 6
    gradient: Gradient {
        GradientStop { id: G1; position: 0.0; color: Palette.lighter(Palette.button) }
        GradientStop { id: G2; position: 1.0; color: Palette.button }
    }

    Text { anchors.centerIn: parent; text: operation; color: Palette.buttonText }

    MouseRegion {
        id: MouseRegion
        anchors.fill: parent
        onClicked: {
            doOp(operation);
            Button.clicked();
            if (!Button.toggable) return;
            Button.toggled ? Button.toggled = false : Button.toggled = true
        }
    }

    states: [
        State {
            name: "Pressed"; when: MouseRegion.pressed == true
            SetProperties { target: G1; color: Palette.dark }
            SetProperties { target: G2; color: Palette.button }
        },
        State {
            name: "Toggled"; when: Button.toggled == true
            SetProperties { target: G1; color: Palette.dark }
            SetProperties { target: G2; color: Palette.button }
        }
    ]
}
