import Qt 4.7

Rectangle {
    id: mainRec
    width:  600
    height: 400
    z: 0

//! [adding a sequential animation]
    Image {
        id: image1
        source: "images/qt-logo.png"
        width: 100; height: 100

        SequentialAnimation on x {
            loops: Animation.Infinite
            NumberAnimation {
                from: 20; to: 450; easing.type: "InOutQuad";
                duration: 2000
            }
            PauseAnimation { duration: 500 }
        }
    }
//! [adding a sequential animation]

    Image {
        id: image2
        source: "images/qt-logo.png"
        width: 100; height: 100
        x: (mainRec.width - 100)/2; y: (mainRec.height - 100)/2
        z: 2
    }
}
