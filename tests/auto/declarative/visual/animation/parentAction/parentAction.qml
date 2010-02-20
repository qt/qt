import Qt 4.6

Rectangle {
    width: 400; height: 400
    Item {
        scale: .5
        rotation: 15
        transformOrigin: "Center"
        x: 10; y: 10
        Rectangle {
            id: myRect
            x: 5
            width: 100; height: 100
            transformOrigin: "BottomLeft"
            color: "red"
        }
    }
    MouseRegion {
        id: clickable
        anchors.fill: parent
    }

    Item {
        x: 200; y: 200
        rotation: 52;
        scale: 2
        Item {
            id: newParent
            x: 100; y: 100
        }
    }

    states: State {
        name: "moved"
        when: clickable.pressed
        ParentChange {
            target: myRect
            parent: newParent
        }
        PropertyChanges {
            target: myRect
            rotation: -52
            scale: 1
            color: "blue"
        }
    }

    transitions: Transition {
        SequentialAnimation {
            ColorAnimation { duration: 500}
            ParentAction {}
            NumberAnimation { properties: "rotation, scale"; duration: 1000 }
        }
    }
}
