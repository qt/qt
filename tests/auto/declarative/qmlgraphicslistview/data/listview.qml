import Qt 4.6

Rectangle {
    width: 240
    height: 320
    color: "#ffffff"
    resources: [
        Component {
            id: myDelegate
            Rectangle {
                id: wrapper
                objectName: "wrapper"
                height: 20
                width: 240
                Text {
                    text: index
                }
                Text {
                    x: 30
                    id: textName
                    objectName: "textName"
                    text: name
                }
                Text {
                    x: 120
                    id: textNumber
                    objectName: "textNumber"
                    text: number
                }
                Text {
                    x: 200
                    text: wrapper.y
                }
                color: ListView.isCurrentItem ? "lightsteelblue" : "white"
            }
        },
        Component {
            id: animatedDelegate
            Rectangle {
                id: wrapper
                objectName: "wrapper"
                height: 20
                width: 240
                Text {
                    text: index
                }
                Text {
                    x: 30
                    id: textName
                    objectName: "textName"
                    text: name
                }
                Text {
                    x: 120
                    id: textNumber
                    objectName: "textNumber"
                    text: number
                }
                Text {
                    x: 200
                    text: wrapper.y
                }
                color: ListView.isCurrentItem ? "lightsteelblue" : "white"
                ListView.onRemove: SequentialAnimation {
                    ScriptAction { script: print("Fix PropertyAction with attached properties") }
/*
                    PropertyAction { target: wrapper; property: "ListView.delayRemove"; value: true }
                    NumberAnimation { target: wrapper; property: "scale"; to: 0; duration: 250; easing: "easeInOutQuad" }
                    PropertyAction { target: wrapper; property: "ListView.delayRemove"; value: false }
*/
                }
            }
        }
    ]
    ListView {
        id: list
        objectName: "list"
        focus: true
        width: 240
        height: 320
        model: testModel
        delegate: testAnimate ? myDelegate : animatedDelegate
        highlightMoveSpeed: 1000
        highlightResizeSpeed: 1000
    }
}
