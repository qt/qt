Item {
    id: Container

    property string label
    property string text

    width: Label.width + Editor.width + 20
    height: Image.height

    states: [
        State {
            name: "Edit"
            SetProperties {
                target: Label
                text: Container.label + ": "
            }
            SetProperties {
                target: Editor
                cursorVisible: true
                width: 100
            }
            SetProperties {
                target: Proxy
                focus: true
            }
            RunScript {
                script:"Editor.selectAll()"
            }
        },
        State {
            // When returning to default state, typed text is propagated
            RunScript {
                script: "Container.text = Editor.text"
            }
        }
    ]
    transitions: [
        Transition {
            ParallelAnimation {
                NumericAnimation { properties: "width"; duration: 500; easing: "easeInOutQuad" }
                SequentialAnimation {
                    PauseAnimation { duration: 100 }
                    SetPropertyAction { properties: "text" }
                }
            }
        }
    ]


    Image {
        id: Image
        source: "pics/button.sci"
        anchors.left: Container.left
        anchors.right: Container.right
    }

    Image {
        id: Pressed
        source: "pics/button-pressed.sci"
        opacity: 0
        anchors.left: Container.left
        anchors.right: Container.right
    }

    MouseRegion {
        id: MouseRegion
        anchors.fill: Image
        onClicked: { Container.state = Container.state=="Edit" ? "" : "Edit" }
        states: [
            State {
                when: MouseRegion.pressed == true
                SetProperties {
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
        anchors.left: Container.left
        anchors.leftMargin: 10
        text: Container.label + "..."
    }

    TextEdit {
        id: Editor
        font.bold: true
        color: "white"
        width: 0
        clip: true
        anchors.left: Label.right
        anchors.verticalCenter: Container.verticalCenter
    }
    KeyProxy {
        id: Proxy
        anchors.left: Container.left
        anchors.fill: Container
        focusable: true
        targets: [(ReturnKey), (Editor)]
    }
    KeyActions {
        id: ReturnKey
        return: "Container.state = ''"
    }
}
