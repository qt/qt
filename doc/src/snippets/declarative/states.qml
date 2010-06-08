//![0]
import Qt 4.7

Item {
    id: myItem
    width: 200; height: 200

    Rectangle {
        id: myRect
        width: 100; height: 100
        color: "red"
    }

    states: [
        State {
            name: "moved"
            PropertyChanges {
                target: myRect
                x: 50
                y: 50
            }
        }
    ]

    MouseArea {
        anchors.fill: parent
        onClicked: myItem.state = 'moved'
    }
//![0]

//![transitions]
transitions: [
    Transition {
        NumberAnimation { properties: "x,y"; duration: 500 }
    }
]
//![transitions]

//![1]
}
//![1]
