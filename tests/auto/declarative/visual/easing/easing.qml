Rect {
    id: item
    width: 600
    height: layout.height
    color: "white"
    resources: [
        ListModel {
            id: easingtypes
            ListElement {
                type: "easeNone"
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
    VerticalLayout {
        id: layout
        anchors.left: item.left
        anchors.right: item.right
        Repeater {
            dataSource: easingtypes
            Component {
                Text {
                    id: text
                    text: type
                    height: 18
                    font.italic: true
                    color: "black"
                    states: [
                        State {
                            name: "from"
                            when: !mouse.pressed
                            SetProperties {
                                target: text
                                x: 0
                            }
                        },
                        State {
                            name: "to"
                            when: mouse.pressed
                            SetProperties {
                                target: text
                                x: item.width-100
                            }
                        }
                    ]
                    transitions: [
                        Transition {
                            fromState: "*"
                            toState: "to"
                            reversible: true
                            NumberAnimation {
                                properties: "x"
                                easing: type
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
