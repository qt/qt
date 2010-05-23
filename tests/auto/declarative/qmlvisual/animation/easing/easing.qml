import Qt 4.7

Rectangle {
    id: item
    width: 600
    height: layout.height
    color: "white"
    resources: [
        ListModel {
            id: easingtypes
            ListElement {
                type: "Linear"
            }
            ListElement {
                type: "InQuad"
            }
            ListElement {
                type: "OutQuad"
            }
            ListElement {
                type: "InOutQuad"
            }
            ListElement {
                type: "OutInQuad"
            }
            ListElement {
                type: "InCubic"
            }
            ListElement {
                type: "OutCubic"
            }
            ListElement {
                type: "InOutCubic"
            }
            ListElement {
                type: "OutInCubic"
            }
            ListElement {
                type: "InQuart"
            }
            ListElement {
                type: "OutQuart"
            }
            ListElement {
                type: "InOutQuart"
            }
            ListElement {
                type: "OutInQuart"
            }
            ListElement {
                type: "InQuint"
            }
            ListElement {
                type: "OutQuint"
            }
            ListElement {
                type: "InOutQuint"
            }
            ListElement {
                type: "OutInQuint"
            }
            ListElement {
                type: "InSine"
            }
            ListElement {
                type: "OutSine"
            }
            ListElement {
                type: "InOutSine"
            }
            ListElement {
                type: "OutInSine"
            }
            ListElement {
                type: "InExpo"
            }
            ListElement {
                type: "OutExpo"
            }
            ListElement {
                type: "InOutExpo"
            }
            ListElement {
                type: "OutInExpo"
            }
            ListElement {
                type: "InCirc"
            }
            ListElement {
                type: "OutCirc"
            }
            ListElement {
                type: "InOutCirc"
            }
            ListElement {
                type: "OutInCirc"
            }
            ListElement {
                type: "InElastic"
            }
            ListElement {
                type: "OutElastic"
            }
            ListElement {
                type: "InOutElastic"
            }
            ListElement {
                type: "OutInElastic"
            }
            ListElement {
                type: "InBack"
            }
            ListElement {
                type: "OutBack"
            }
            ListElement {
                type: "InOutBack"
            }
            ListElement {
                type: "OutInBack"
            }
            ListElement {
                type: "OutBounce"
            }
            ListElement {
                type: "InBounce"
            }
            ListElement {
                type: "InOutBounce"
            }
            ListElement {
                type: "OutInBounce"
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
                                properties: "x"
                                easing.type: type
                                duration: 1000
                            }
                        }
                    ]
                }
            }
        }
    }
    MouseArea {
        id: mouse
        anchors.fill: layout
    }
}
