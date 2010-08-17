import Qt 4.7

Rectangle {
    id: mainRectangle
    width:  600
    height: 400
    color: "black"

    Rectangle {
        id: sky
        width: 600
        height: 200
        y: 0
        color: "lightblue"
    }

    Rectangle {
        id: ground
        width: 600; height: 200
        y: 200
        color: "green"
    }

    MouseArea {
        id: mousearea
        anchors.fill: mainRectangle
    }

    states: [ State {
            name: "night"
            when: mousearea.pressed == true
            PropertyChanges { target: sky; color: "darkblue" }
            PropertyChanges { target: ground; color: "black" }
        },
        State {
            name: "daylight"
            when: mousearea.pressed == false
            PropertyChanges { target: sky; color: "lightblue" }
            PropertyChanges { target: ground; color: "green" }
        }
    ]

    transitions: [ Transition {
            from: "daylight"; to: "night"
            ColorAnimation { duration: 1000 }
        },
        Transition {
            from: "night"; to: "daylight"
            ColorAnimation { duration: 500 }
        }
    ]
}
