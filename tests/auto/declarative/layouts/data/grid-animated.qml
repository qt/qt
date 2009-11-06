import Qt 4.6

Item {
    width: 640
    height: 480
    Grid {
        columns: 3
        add: Transition {
            NumberAnimation {
                matchProperties: "x,y"; from: -100
            }
        }
        remove: Transition {
            NumberAnimation {
                matchProperties: "x,y"; to: -100
            }
        }
        move: Transition {
            NumberAnimation {
                matchProperties: "x,y";
            }
        }
        Rectangle {
            objectName: "one"
            color: "red"
            width: 50
            height: 50
        }
        Rectangle {
            objectName: "two"
            opacity: 0
            color: "green"
            width: 50
            height: 50
        }
        Rectangle {
            objectName: "three"
            color: "blue"
            width: 50
            height: 50
        }
        Rectangle {
            objectName: "four"
            color: "cyan"
            width: 50
            height: 50
        }
        Rectangle {
            objectName: "five"
            color: "magenta"
            width: 50
            height: 50
        }
    }
}
