import Qt 4.6

Rectangle {
    color: "lightsteelblue"
    width: 800
    height: 600
    id: page
    MouseArea {
        anchors.fill: parent
        onClicked: { bluerect.parent = page; console.log(mouseX); bluerect.x = mouseX; }
    }
    MyRect {
        color: "green"
        x: 200
        y: 200
    }
    MyRect {
        color: "red"
        x: 400
        y: 200
    }
    MyRect {
        color: "yellow"
        x: 400
        y: 400
    }
    MyRect {
        color: "orange"
        x: 400
        y: 500
    }
    MyRect {
        color: "pink"
        x: 400
        y: 0
    }
    MyRect {
        color: "lightsteelblue"
        x: 100
        y: 500
    }
    MyRect {
        color: "black"
        x: 0
        y: 200
    }
    MyRect {
        color: "white"
        x: 400
        y: 0
    }
    Rectangle {
        color: "blue"
        x: 0
        y: 0
        width: 100
        height: 100
        id: bluerect
        x: Behavior {
            ParallelAnimation {
                SequentialAnimation {
                    NumberAnimation {
                        target: bluerect
                        property: "y"
                        from: 0
                        to: 10
                        easing.type: "OutBounce"
                        easing.amplitude: 30
                        duration: 250
                    }
                    NumberAnimation {
                        target: bluerect
                        property: "y"
                        from: 10
                        to: 0
                        easing.type: "OutBounce"
                        easing.amplitude: 30
                        duration: 250
                    }
                }
                NumberAnimation { duration: 500 }
            }
        }
        parent: Behavior {
            SequentialAnimation {
                NumberAnimation {
                    target: bluerect
                    property: "opacity"
                    to: 0
                    duration: 150
                }
                PropertyAction {}
                NumberAnimation {
                    target: bluerect
                    property: "opacity"
                    to: 1
                    duration: 150
                }
            }
        }
    }
}
