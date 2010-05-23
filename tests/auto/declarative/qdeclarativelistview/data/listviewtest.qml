import Qt 4.7

Rectangle {
    width: 240
    height: 320
    color: "#ffffff"
    function checkProperties() {
        testObject.error = false;
        if (list.model != testModel) {
            console.log("model property incorrect");
            testObject.error = true;
        }
        if (!testObject.animate && list.delegate != myDelegate) {
            console.log("delegate property incorrect - expected myDelegate");
            testObject.error = true;
        }
        if (testObject.animate && list.delegate != animatedDelegate) {
            console.log("delegate property incorrect - expected animatedDelegate");
            testObject.error = true;
        }
        if (testObject.invalidHighlight && list.highlight != invalidHl) {
            console.log("highlight property incorrect - expected invalidHl");
            testObject.error = true;
        }
        if (!testObject.invalidHighlight && list.highlight != myHighlight) {
            console.log("highlight property incorrect - expected myHighlight");
            testObject.error = true;
        }
    }
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
                    PropertyAction { target: wrapper; property: "ListView.delayRemove"; value: true }
                    NumberAnimation { target: wrapper; property: "scale"; to: 0; duration: 250; easing.type: "InOutQuad" }
                    PropertyAction { target: wrapper; property: "ListView.delayRemove"; value: false }

                }
            }
        },
        Component {
            id: myHighlight
            Rectangle { color: "green" }
        },
        Component {
            id: invalidHl
            SmoothedAnimation {}
        }
    ]
    ListView {
        id: list
        objectName: "list"
        focus: true
        width: 240
        height: 320
        model: testModel
        delegate: testObject.animate ? animatedDelegate : myDelegate
        highlight: testObject.invalidHighlight ? invalidHl : myHighlight
        highlightMoveSpeed: 1000
        highlightResizeSpeed: 1000
        cacheBuffer: testObject.cacheBuffer
    }
}
