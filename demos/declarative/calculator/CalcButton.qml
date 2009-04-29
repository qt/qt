Item {
    id: Button; width: 50; height: 30

    properties: [
        Property { name: "operation"; type: "string" },
        Property { name: "toggable"; value: false },
        Property { name: "toggled"; value: false }
    ]

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
        src: "pics/button.sci"
        width: Button.width; height: Button.height
    }

    Image {
        id: ImagePressed
        src: "pics/button-pressed.sci"
        width: Button.width; height: Button.height
        opacity: 0
    }

    Text {
        anchors.centeredIn: Image
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
