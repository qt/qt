import Qt 4.7

Rectangle {
    id: mainRec
    width:  600
    height: 400
    z: 0

    Image {
        id: image1
        source: "images/qt-logo.png"
        x: 20; y: 20 ; z: 1
        width: 100; height: 100
    }

    Image {
        id: image2
        source: "images/qt-logo.png"
        width: 100; height: 100
        x: (mainRec.width - 100)/2; y: (mainRec.height - 100)/2
        z: 2
    }
}
