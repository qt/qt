import Qt 4.7

/*
    This is exactly the same as states.qml, except that we have appended
    a set of transitions to apply animations when the item changes 
    between each state.
*/

Rectangle {
    id: page
    width: 640; height: 480
    color: "#343434"

    Image { 
        id: userIcon
        x: topLeftRect.x; y: topLeftRect.y
        source: "user.png"
    }

    Rectangle {
        id: topLeftRect

        anchors { left: parent.left; top: parent.top; leftMargin: 10; topMargin: 20 }
        width: 64; height: 64
        color: "Transparent"; border.color: "Gray"; radius: 6

        // Clicking in here sets the state to the default state, returning the image to
        // its initial position
        MouseArea { anchors.fill: parent; onClicked: page.state = '' }
    }

    Rectangle {
        id: middleRightRect

        anchors { right: parent.right; verticalCenter: parent.verticalCenter; rightMargin: 20 }
        width: 64; height: 64
        color: "Transparent"; border.color: "Gray"; radius: 6

        // Clicking in here sets the state to 'middleRight'
        MouseArea { anchors.fill: parent; onClicked: page.state = 'middleRight' }
    }

    Rectangle {
        id: bottomLeftRect

        anchors { left: parent.left; bottom: parent.bottom; leftMargin: 10; bottomMargin: 20 }
        width: 64; height: 64
        color: "Transparent"; border.color: "Gray"; radius: 6

        // Clicking in here sets the state to 'bottomLeft'
        MouseArea { anchors.fill: parent; onClicked: page.state = 'bottomLeft' }
    }

    states: [
        // In state 'middleRight', move the image to middleRightRect
        State {
            name: "middleRight"
            PropertyChanges { target: userIcon; x: middleRightRect.x; y: middleRightRect.y }
        },

        // In state 'bottomLeft', move the image to bottomLeftRect
        State {
            name: "bottomLeft"
            PropertyChanges { target: userIcon; x: bottomLeftRect.x; y: bottomLeftRect.y  }
        }
    ]

    // Transitions define how the properties change when the item moves between each state
    transitions: [

        // When transitioning to 'middleRight' move x,y over a duration of 1 second,
        // with OutBounce easing function.
        Transition {
            from: "*"; to: "middleRight"
            NumberAnimation { properties: "x,y"; easing.type: "OutBounce"; duration: 1000 }
        },

        // When transitioning to 'bottomLeft' move x,y over a duration of 2 seconds,
        // with InOutQuad easing function.
        Transition {
            from: "*"; to: "bottomLeft"
            NumberAnimation { properties: "x,y"; easing.type: "InOutQuad"; duration: 2000 }
        },

        // For any other state changes move x,y linearly over duration of 200ms.
        Transition {
            NumberAnimation { properties: "x,y"; duration: 200 }
        }
    ]
}
