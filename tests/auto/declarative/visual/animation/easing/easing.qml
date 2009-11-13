import Qt 4.6

Rectangle {
    id: item
    width: 600
    height: layout.height
    color: "white"
    resources: [
        ListModel {
            id: easingtypes
            ListElement {
                type: "easeLinear"
            }
            ListElement {
                type: "easeInQuad"
            }
            ListElement {
                type: "easeOutQuad"
            }
            ListElement {
                type: "easeInOutQuad"
            }
            ListElement {
                type: "easeOutInQuad"
            }
            ListElement {
                type: "easeInCubic"
            }
            ListElement {
                type: "easeOutCubic"
            }
            ListElement {
                type: "easeInOutCubic"
            }
            ListElement {
                type: "easeOutInCubic"
            }
            ListElement {
                type: "easeInQuart"
            }
            ListElement {
                type: "easeOutQuart"
            }
            ListElement {
                type: "easeInOutQuart"
            }
            ListElement {
                type: "easeOutInQuart"
            }
            ListElement {
                type: "easeInQuint"
            }
            ListElement {
                type: "easeOutQuint"
            }
            ListElement {
                type: "easeInOutQuint"
            }
            ListElement {
                type: "easeOutInQuint"
            }
            ListElement {
                type: "easeInSine"
            }
            ListElement {
                type: "easeOutSine"
            }
            ListElement {
                type: "easeInOutSine"
            }
            ListElement {
                type: "easeOutInSine"
            }
            ListElement {
                type: "easeInExpo"
            }
            ListElement {
                type: "easeOutExpo"
            }
            ListElement {
                type: "easeInOutExpo"
            }
            ListElement {
                type: "easeOutInExpo"
            }
            ListElement {
                type: "easeInCirc"
            }
            ListElement {
                type: "easeOutCirc"
            }
            ListElement {
                type: "easeInOutCirc"
            }
            ListElement {
                type: "easeOutInCirc"
            }
            ListElement {
                type: "easeInElastic"
            }
            ListElement {
                type: "easeOutElastic"
            }
            ListElement {
                type: "easeInOutElastic"
            }
            ListElement {
                type: "easeOutInElastic"
            }
            ListElement {
                type: "easeInBack"
            }
            ListElement {
                type: "easeOutBack"
            }
            ListElement {
                type: "easeInOutBack"
            }
            ListElement {
                type: "easeOutInBack"
            }
            ListElement {
                type: "easeOutBounce"
            }
            ListElement {
                type: "easeInBounce"
            }
            ListElement {
                type: "easeInOutBounce"
            }
            ListElement {
                type: "easeOutInBounce"
            }
        }
    ]
    Column {
        id: layout
        anchors.left: item.left
        anchors.right: item.right
        Repeater {
            model: easingtypes
            Component {
                Rectangle {
                    id: block
                    Text {
                        text: type
                        anchors.centerIn: parent
                        font.italic: true
                        color: index & 1 ? "black" : "white"
                        opacity: 0 // 1 for debugging
                    }
                    width: 120
                    height: 18
                    color: index & 1 ? "red" : "blue"
                    states: [
                        State {
                            name: "from"
                            when: !mouse.pressed
                            PropertyChanges {
                                target: block
                                x: 0
                            }
                        },
                        State {
                            name: "to"
                            when: mouse.pressed
                            PropertyChanges {
                                target: block
                                x: item.width-block.width
                            }
                        }
                    ]
                    transitions: [
                        Transition {
                            from: "*"
                            to: "to"
                            reversible: true
                            NumberAnimation {
                                matchProperties: "x"
                                easing: type
                                duration: 1000
                            }
                        }
                    ]
                }
            }
        }
    }
    MouseRegion {
        id: mouse
        anchors.fill: layout
    }
}
