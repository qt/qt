import Qt 4.7

Rectangle {
    id: mainRec
    width:  600
    height: 400

    Image {
        id: image1
        source: "images/qt-logo.png"
        x: 200; y: 100
        width: 100; height: 100

        // Animate a rotation
        transform: Rotation {
            origin.x: 50; origin.y: 50; axis {x:0; y:1; z:0} angle:0
            NumberAnimation on angle  {
                from: 0; to: 360;
                duration: 3000;
                loops: Animation.Infinite
            }
        }
    }
}
