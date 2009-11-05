import Qt 4.6

Item {
    width: 640
    height: 480
    Column {
        add: Transition {
            NumberAnimation {
                properties: "y"; from: -100
            }
        }
        remove: Transition {
            NumberAnimation {
                properties: "y"; to: -100
            }
        }
        move: Transition {
            NumberAnimation {
                properties: "y";
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
