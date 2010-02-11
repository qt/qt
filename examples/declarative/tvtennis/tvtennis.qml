import Qt 4.6

Rectangle {
    id: page
    width: 640; height: 480
    color: "Black"

    // Make a ball to bounce
    Rectangle {
        // Add a property for the target y coordinate
        property int targetY : page.height - 10
        property var direction : "right"

        id: ball
        color: "Lime"
        x: 20; width: 20; height: 20; z: 1

        // Move the ball to the right and back to the left repeatedly
        x: SequentialAnimation {
            repeat: true
            NumberAnimation { to: page.width - 40; duration: 2000 }
            ScriptAction { script: Qt.playSound('paddle.wav') }
            PropertyAction { target: ball; property: "direction"; value: "left" }
            NumberAnimation { to: 20; duration: 2000 }
            ScriptAction { script: Qt.playSound('paddle.wav') }
            PropertyAction { target: ball; property: "direction"; value: "right" }
        }

        // Make y follow the target y coordinate, with a velocity of 200
        y: SpringFollow { source: ball.targetY; velocity: 200 }

        // Detect the ball hitting the top or bottom of the view and bounce it
        onYChanged: {
            if (y <= 0) {
                Qt.playSound('click.wav');
                targetY = page.height - 20;
            } else if (y >= page.height - 20) {
                Qt.playSound('click.wav');
                targetY = 0;
            }
        }
    }

    // Place bats to the left and right of the view, following the y
    // coordinates of the ball.
    Rectangle {
        id: leftBat
        color: "Lime"
        x: 2; width: 20; height: 90
        y: SpringFollow {
            source: ball.y - 45; velocity: 300
            enabled: ball.direction == 'left'
        }
    }
    Rectangle {
        id: rightBat
        color: "Lime"
        x: page.width - 22; width: 20; height: 90
        y: SpringFollow {
            source: ball.y-45; velocity: 300
            enabled: ball.direction == 'right'
        }
    }

    // The rest, to make it look realistic, if neither ever scores...
    Rectangle { color: "Lime"; x: page.width/2-80; y: 0; width: 40; height: 60 }
    Rectangle { color: "Black"; x: page.width/2-70; y: 10; width: 20; height: 40 }
    Rectangle { color: "Lime"; x: page.width/2+40; y: 0; width: 40; height: 60 }
    Rectangle { color: "Black"; x: page.width/2+50; y: 10; width: 20; height: 40 }
    Repeater {
        model: page.height / 20
        Rectangle { color: "Lime"; x: page.width/2-5; y: index * 20; width: 10; height: 10 }
    }
}
