import Qt 4.6

Item {
    width: 640
    height: 480
    Row {
        add: Transition {
            NumberAnimation {
                matchProperties: "x"; from: -100
            }
        }
        remove: Transition {
            NumberAnimation {
                matchProperties: "x"; to: -100
            }
        }
        move: Transition {
            NumberAnimation {
                matchProperties: "x";
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
            color: "blue"
            opacity: 0
            width: 50
            height: 50
        }
        Rectangle {
            objectName: "three"
            color: "red"
            width: 50
            height: 50
        }
    }
}
