import Qt 4.6

Rectangle {
    id: Page
    width: 640; height: 480
    color: "#000000"

    // Make a ball to bounce
    Rectangle {
        // Add a property for the target y coordinate
        property var targetY : Page.height-10
        property var direction : "right"

        id: Ball
        color: "#00ee00"
        x: 20; width: 20; height: 20; z: 1

        // Move the ball to the right and back to the left repeatedly
        x: SequentialAnimation {
            running: true; repeat: true
            NumberAnimation { to: Page.width-40; duration: 2000 }
            PropertyAction { target: Ball; property: "direction"; value: "left" }
            NumberAnimation { to: 20; duration: 2000 }
            PropertyAction { target: Ball; property: "direction"; value: "right" }
        }

        // Make y follow the target y coordinate, with a velocity of 200
        y: Follow { source: Ball.targetY; velocity: 200 }

        // Detect the ball hitting the top or bottom of the view and bounce it
        onYChanged: {
            if (y <= 0)
                targetY = Page.height-20;
            else if (y >= Page.height-20)
                targetY = 0;
        }
    }

    // Place bats to the left and right of the view, following the y
    // coordinates of the ball.
    Rectangle {
        id: LeftBat
        color: "#00ee00"
        x: 2; width: 20; height: 90
        y: Follow {
            source: Ball.y-45; velocity: 300
            enabled: Ball.direction == 'left'
        }
    }
    Rectangle {
        id: RightBat
        color: "#00ee00"
        x: Page.width-22; width: 20; height: 90
        y: Follow {
            source: Ball.y-45; velocity: 300
            enabled: Ball.direction == 'right'
        }
    }

    // The rest, to make it look realistic, if neither ever scores...
    Rectangle { color: "#00ee00"; x: Page.width/2-80; y: 0; width: 40; height: 60 }
    Rectangle { color: "#000000"; x: Page.width/2-70; y: 10; width: 20; height: 40 }
    Rectangle { color: "#00ee00"; x: Page.width/2+40; y: 0; width: 40; height: 60 }
    Rectangle { color: "#000000"; x: Page.width/2+50; y: 10; width: 20; height: 40 }
    Repeater {
        dataSource: Page.height/20
        Rectangle { color: "#00ee00"; x: Page.width/2-5; y: index*20; width: 10; height: 10 }
    }
}
