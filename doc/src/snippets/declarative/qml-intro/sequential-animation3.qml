Rectangle {
    id: mainRec
    width:  600
    height: 400
    z: 0

    Image {
        id: image2
        source: "images/qt-logo.png"
        width: 100; height: 100
        x: (mainRec.width - 100)/2; y: (mainRec.height - 100)/2
        z: 2
    }

    Image {
        id: image1
        source: "images/qt-logo.png"
        x: 20; y: 20 ; z: 1
        width: 100; height: 100

        SequentialAnimation on x {
            loops: Animation.Infinite
            NumberAnimation {
                from: 20; to: 450
                easing.type: "InOutQuad"; duration: 2000
            }
            PauseAnimation { duration: 500 }
        }

        SequentialAnimation on y {
            loops: Animation.Infinite
            NumberAnimation {
                from: 20; to: 250
                easing.type: "InOutQuad"; duration: 2000
            }
            PauseAnimation { duration: 500 }
        }

        SequentialAnimation on scale {
            loops: Animation.Infinite
            NumberAnimation { from: 1; to: 0.5; duration: 1000 }
            NumberAnimation { from: 0.5; to: 1; duration: 1000 }
            PauseAnimation { duration: 500 }
        }
    }
}
