import Qt 4.6

Rectangle {
    id: newParent
    width: 400; height: 400
    Item {
        scale: .5
        rotation: 15
        x: 10; y: 10
        Rectangle {
            id: myRect
            objectName: "MyRect"
            x: 5
            width: 100; height: 100
            color: "red"
        }
    }
    MouseRegion {
        id: Clickable
        anchors.fill: parent
    }

    states: State {
        name: "reparented"
        when: Clickable.pressed
        ParentChange {
            target: myRect
            parent: newParent
        }
    }
}
