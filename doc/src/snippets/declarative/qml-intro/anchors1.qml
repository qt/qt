import Qt 4.7

Rectangle {
    id: myWin
    width: 500
    height: 400

    Image {
        id: image1
        source: "images/qt-logo.png"
        width: 150; height: 150
        anchors.bottom: myWin.bottom
    }
}
