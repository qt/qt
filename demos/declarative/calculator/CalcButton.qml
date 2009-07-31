import Qt 4.6

Item {
    property string operation
    property bool toggable : false
    property bool toggled : false

    id: Button; width: 50; height: 30

    Script {
        function buttonClicked(operation) {
            if (Button.toggable == true) {
                if (Button.toggled == true) {
                    Button.toggled = false;
                    Button.state = 'Toggled';
                } else {
                    Button.toggled = true;
                    Button.state = '';
                }
            }
            else
                doOp(operation);
        }
    }

    Image {
        id: Image
        source: "pics/button.sci"
        width: Button.width; height: Button.height
    }

    Image {
        id: ImagePressed
        source: "pics/button-pressed.sci"
        width: Button.width; height: Button.height
        opacity: 0
    }

    Text {
        anchors.centerIn: Image
        text: Button.operation
        color: "white"
        font.bold: true
    }

    MouseRegion {
        id: MouseRegion
        anchors.fill: Button
        onClicked: { buttonClicked(Button.operation) }
    }

    states: [
        State {
            name: "Pressed"; when: MouseRegion.pressed == true
            SetProperties { target: ImagePressed; opacity: 1 }
        },
        State {
            name: "Toggled"; when: Button.toggled == true
            SetProperties { target: ImagePressed; opacity: 1 }
        }
    ]
}
