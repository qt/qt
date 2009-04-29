Rect {
    id: Page
    width: 300
    height: 300
    color: "white"
    // A target region.  Clicking in here sets the state to '' - the default state
    Rect {
        width: 50
        height: 50
        x: 0
        y: 0
        color: "transparent"
        pen.color: "black"
        MouseRegion {
            anchors.fill: parent
            onClicked: { Page.state='' }
        }
    }
    // Another target region.  Clicking in here sets the state to 'Position1'
    Rect {
        width: 50
        height: 50
        x: 150
        y: 50
        color: "transparent"
        pen.color: "black"
        MouseRegion {
            anchors.fill: parent
            onClicked: { Page.state='Position1' }
        }
    }
    // Another target region.  Clicking in here sets the state to 'Position2'
    Rect {
        width: 50
        height: 50
        x: 0
        y: 200
        color: "transparent"
        pen.color: "black"
        MouseRegion {
            anchors.fill: parent
            onClicked: { Page.state='Position2' }
        }
    }
    // Rect which will be moved when my state changes
    Rect {
        id: myrect
        width: 50
        height: 50
        color: "red"
    }
    states: [
        // In state 'Position1', change the 'myrect' item x, y to 150, 50.
        State {
            name: "Position1"
            SetProperty {
                target: myrect
                property: "x"
                value: 150
            }
            SetProperty {
                target: myrect
                property: "y"
                value: 50
            }
        }        // In state 'Position2', change y to 100.  We do not specify 'x' here - 
        // it will therefore be restored to its default value of 0, if it
        // had been changed.
,
        State {
            name: "Position2"
            SetProperty {
                target: myrect
                property: "y"
                value: 200
            }
        }
    ]
}
