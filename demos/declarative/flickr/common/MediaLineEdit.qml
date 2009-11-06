import Qt 4.6

Item {
    id: container

    property string label
    property string text

    width: Math.max(94,labeltext.width + editor.width + 20)
    height: buttonImage.height

    states: [
        State {
            name: "Edit"
            PropertyChanges {
                target: labeltext
                text: container.label + ": "
            }
            PropertyChanges {
                target: labeltext
                x: 10
            }
            PropertyChanges {
                target: editor
                cursorVisible: true
                width: 100
            }
            PropertyChanges {
                target: container
                focus: true
            }
            StateChangeScript {
                script:editor.selectAll()
            }
        },
        State {
            // When returning to default state, typed text is propagated
            StateChangeScript {
                script: container.text = editor.text
            }
        }
    ]
    transitions: [
        Transition {
            NumberAnimation { matchProperties: "x,width"; duration: 500; easing: "easeInOutQuad" }
        }
    ]


    BorderImage {
        id: buttonImage
        source: "pics/button.sci"
        anchors.left: container.left
        anchors.right: container.right
    }

    BorderImage {
        id: pressed
        source: "pics/button-pressed.sci"
        opacity: 0
        anchors.left: container.left
        anchors.right: container.right
    }

    MouseRegion {
        id: mouseRegion
        anchors.fill: buttonImage
        onClicked: { container.state = container.state=="Edit" ? "" : "Edit" }
        states: [
            State {
                when: mouseRegion.pressed == true
                PropertyChanges {
                    target: pressed
                    opacity: 1
                }
            }
        ]
    }

    Text {
        id: labeltext
        font.bold: true
        color: "white"
        anchors.verticalCenter: container.verticalCenter
        x: (container.width - width)/2
        text: container.label + "..."
    }

    TextInput {
        id: editor
        font.bold: true
        color: "white"
        selectionColor: "green"
        width: 0
        clip: true
        anchors.left: labeltext.right
        anchors.verticalCenter: container.verticalCenter
    }
    Keys.forwardTo: [(returnKey), (editor)]
    Item {
        id: returnKey
        Keys.onReturnPressed: "container.state = ''"
    }
}
