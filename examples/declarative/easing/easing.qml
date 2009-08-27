import Qt 4.6

Rectangle {
    id: Window
    width: 640
    height: Layout.height
    color: "white"

    ListModel {
        id: EasingTypes
        ListElement { type: "easeLinear" }
        ListElement { type: "easeInQuad" }
        ListElement { type: "easeOutQuad" }
        ListElement { type: "easeInOutQuad" }
        ListElement { type: "easeOutInQuad" }
        ListElement { type: "easeInCubic" }
        ListElement { type: "easeOutCubic" }
        ListElement { type: "easeInOutCubic" }
        ListElement { type: "easeOutInCubic" }
        ListElement { type: "easeInQuart" }
        ListElement { type: "easeOutQuart" }
        ListElement { type: "easeInOutQuart" }
        ListElement { type: "easeOutInQuart" }
        ListElement { type: "easeInQuint" }
        ListElement { type: "easeOutQuint" }
        ListElement { type: "easeInOutQuint" }
        ListElement { type: "easeOutInQuint" }
        ListElement { type: "easeInSine" }
        ListElement { type: "easeOutSine" }
        ListElement { type: "easeInOutSine" }
        ListElement { type: "easeOutInSine" }
        ListElement { type: "easeInExpo" }
        ListElement { type: "easeOutExpo" }
        ListElement { type: "easeInOutExpo" }
        ListElement { type: "easeOutInExpo" }
        ListElement { type: "easeInCirc" }
        ListElement { type: "easeOutCirc" }
        ListElement { type: "easeInOutCirc" }
        ListElement { type: "easeOutInCirc" }
        ListElement { type: "easeInElastic" }
        ListElement { type: "easeOutElastic" }
        ListElement { type: "easeInOutElastic" }
        ListElement { type: "easeOutInElastic" }
        ListElement { type: "easeInBack" }
        ListElement { type: "easeOutBack" }
        ListElement { type: "easeInOutBack" }
        ListElement { type: "easeOutInBack" }
        ListElement { type: "easeOutBounce" }
        ListElement { type: "easeInBounce" }
        ListElement { type: "easeInOutBounce" }
        ListElement { type: "easeOutInBounce" }
    }
    
    Column {
        id: Layout
        anchors.left: Window.left
        anchors.right: Window.right
        Repeater {
            model: EasingTypes
            Component {
                Text {
                    id: Text
                    text: type
                    height: 18
                    font.italic: true
                    x: SequentialAnimation {
                        id: Anim
                        NumberAnimation {
                            from: 0
                            to: Window.width / 2
                            easing: type
                            duration: 1000
                        }
                        PauseAnimation {
                            duration: 300
                        }
                        NumberAnimation {
                            to: 0
                            from: Window.width / 2
                            easing: type
                            duration: 1000
                        }
                    }
                    children: [
                        MouseRegion {
                            onClicked: { Anim.running=true }
                            anchors.fill: parent
                        }
                    ]
                }
            }
        }
    }
}
