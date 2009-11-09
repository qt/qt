import Qt 4.6

Item {
    width: 640
    height: 480
    Column {
        add: Transition {
            NumberAnimation {
                matchProperties: "y"; from: -100
            }
        }
        remove: Transition {
            NumberAnimation {
                matchProperties: "y"; to: -100
            }
        }
        move: Transition {
            NumberAnimation {
                matchProperties: "y";
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
