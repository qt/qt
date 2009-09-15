import Qt 4.6

Item {
    id: Container

    property string label
    property string text

    width: Math.max(94,Label.width + Editor.width + 20)
    height: ButtonImage.height

    states: [
        State {
            name: "Edit"
            PropertyChanges {
                target: Label
                text: Container.label + ": "
            }
            PropertyChanges {
                target: Label
                x: 10
            }
            PropertyChanges {
                target: Editor
                cursorVisible: true
                width: 100
            }
            PropertyChanges {
                target: Container
                focus: true
            }
            StateChangeScript {
                script:"Editor.selectAll()"
            }
        },
        State {
            // When returning to default state, typed text is propagated
            StateChangeScript {
                script: "Container.text = Editor.text"
            }
        }
    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "x,width"; duration: 500; easing: "easeInOutQuad" }
        }
    ]


    BorderImage {
        id: ButtonImage
        source: "pics/button.sci"
        anchors.left: Container.left
        anchors.right: Container.right
    }

    BorderImage {
        id: Pressed
        source: "pics/button-pressed.sci"
        opacity: 0
        anchors.left: Container.left
        anchors.right: Container.right
    }

    MouseRegion {
        id: MyMouseRegion
        anchors.fill: ButtonImage
        onClicked: { Container.state = Container.state=="Edit" ? "" : "Edit" }
        states: [
            State {
                when: MyMouseRegion.pressed == true
                PropertyChanges {
                    target: Pressed
                    opacity: 1
                }
            }
        ]
    }

    Text {
        id: Label
        font.bold: true
        color: "white"
        anchors.verticalCenter: Container.verticalCenter
        x: (Container.width - width)/2
        text: Container.label + "..."
    }

    TextInput {
        id: Editor
        font.bold: true
        color: "white"
        selectionColor: "green"
        width: 0
        clip: true
        anchors.left: Label.right
        anchors.verticalCenter: Container.verticalCenter
    }
    Keys.forwardTo: [(ReturnKey), (Editor)]
    Item {
        id: ReturnKey
        Keys.onReturnPressed: "Container.state = ''"
    }
}
