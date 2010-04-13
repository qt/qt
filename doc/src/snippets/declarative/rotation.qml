import Qt 4.7

Rectangle {
    width: 360; height: 80
    color: "white"
//! [0]
    Row {
        x: 10; y: 10
        spacing: 10
        Image { source: "pics/qt.png" }
        Image {
            source: "pics/qt.png"
            transform: Rotation { origin.x: 30; origin.y: 30; axis { x: 0; y: 1; z: 0 } angle: 18 }
            smooth: true
        }
        Image {
            source: "pics/qt.png"
            transform: Rotation { origin.x: 30; origin.y: 30; axis { x: 0; y: 1; z: 0 } angle: 36 }
            smooth: true
        }
        Image {
            source: "pics/qt.png"
            transform: Rotation { origin.x: 30; origin.y: 30; axis { x: 0; y: 1; z: 0 } angle: 54 }
            smooth: true
        }
        Image {
            source: "pics/qt.png"
            transform: Rotation { origin.x: 30; origin.y: 30; axis { x: 0; y: 1; z: 0 } angle: 72 }
            smooth: true
        }
    }
//! [0]
}
