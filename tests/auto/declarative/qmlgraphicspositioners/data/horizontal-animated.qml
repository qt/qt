import Qt 4.6

Item {
    width: 640
    height: 480
    Row {
        add: Transition {
            NumberAnimation {
                matchProperties: "x";
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
            x: -100;
            width: 50
            height: 50
        }
        Rectangle {
            objectName: "two"
            color: "blue"
            x: -100;
            opacity: 0
            width: 50
            height: 50
        }
        Rectangle {
            objectName: "three"
            x: -100;
            color: "green"
            width: 50
            height: 50
        }
    }
}
