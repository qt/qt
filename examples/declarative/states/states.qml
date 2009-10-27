import Qt 4.6

Rectangle {
    id: page
    width: 640; height: 480; color: "#343434"

    // A target region.  Clicking in here sets the state to the default state
    Rectangle {
        id: initialPosition
        anchors { left: parent.left; top: parent.top; leftMargin: 10; topMargin: 20 }
        width: 64; height: 64; radius: 6
        color: "Transparent"; border.color: "Gray"
        MouseRegion { anchors.fill: parent; onClicked: page.state = '' }
    }

    // Another target region.  Clicking in here sets the state to 'Position1'
    Rectangle {
        id: position1
        anchors { right: parent.right; verticalCenter: parent.verticalCenter; rightMargin: 20 }
        width: 64; height: 64; radius: 6
        color: "Transparent"; border.color: "Gray"
        MouseRegion { anchors.fill: parent; onClicked: page.state = 'Position1' }
    }

    // Another target region.  Clicking in here sets the state to 'Position2'
    Rectangle {
        id: position2
        anchors { left: parent.left; bottom: parent.bottom; leftMargin: 10; bottomMargin: 20 }
        width: 64; height: 64; radius: 6
        color: "Transparent"; border.color: "Gray"
        MouseRegion { anchors.fill: parent; onClicked: page.state = 'Position2' }
    }

    // The image which will be moved when my state changes
    Image { id: user; source: "user.png"; x: initialPosition.x; y: initialPosition.y }

    states: [
        // In state 'Position1', change the 'user' item position to rect2's position.
        State {
            name: "Position1"
            PropertyChanges { target: user; x: position1.x; y: position1.y }
        },

        // In state 'Position2', change the 'user' item position to rect3's position.
        State {
            name: "Position2"
            PropertyChanges { target: user; x: position2.x; y: position2.y  }
        }
    ]
}
