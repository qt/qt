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
        transformOrigin: Item.Center
        NumberAnimation on rotation {
            from: 0; to: 360
            duration: 2000
            loops: Animation.Infinite
        }
    }
}
