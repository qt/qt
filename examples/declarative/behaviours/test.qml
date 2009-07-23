import Qt 4.6

Rect {
    color: "lightsteelblue"
    width: 800
    height: 600
    id: Page
    MouseRegion {
        anchors.fill: parent
        onClicked: { bluerect.parent = Page; bluerect.x = mouseX; }
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
    Rect {
        color: "blue"
        x: 0
        y: 0
        width: 100
        height: 100
        id: bluerect
        x: Behavior {
            SequentialAnimation {
                NumberAnimation {
                    target: bluerect
                    properties: "y"
                    from: 0
                    to: 10
                    easing: "easeOutBounce(amplitude:30)"
                    duration: 250
                }
                NumberAnimation {
                    target: bluerect
                    properties: "y"
                    from: 10
                    to: 0
                    easing: "easeOutBounce(amplitude:30)"
                    duration: 250
                }
            }
            NumberAnimation { duration: 500 }
        }
        parent: Behavior {
            SequentialAnimation {
                NumberAnimation {
                    target: bluerect
                    properties: "opacity"
                    to: 0
                    duration: 150
                }
                SetPropertyAction {}
                NumberAnimation {
                    target: bluerect
                    properties: "opacity"
                    to: 1
                    duration: 150
                }
            }
        }
    }
}
