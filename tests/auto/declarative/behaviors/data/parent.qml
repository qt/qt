import Qt 4.6
Rectangle {
    width: 400
    height: 400
    Rectangle {
        id: rect
        objectName: "MyRect"
        width: 100; height: 100; color: "green"
        parent: Behavior {
            SequentialAnimation {
                PauseAnimation { duration: 200 }
                PropertyAction {}
            }
        }
    }
    Item {
        id: newParent
        objectName: "NewParent"
        x: 100
    }
    states: State {
        name: "reparented"
        PropertyChanges {
            target: rect
            parent: newParent
        }
    }
}
