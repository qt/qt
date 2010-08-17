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
        anchors.horizontalCenter: myWin.horizontalCenter
        anchors.bottomMargin: 10
    }

//! [adding some text]
    Text {
        text: "<h2>The Qt Logo</h2>"
        anchors.bottom: image1.top
        anchors.horizontalCenter: myWin.horizontalCenter
        anchors.bottomMargin: 15
    }
//! [adding some text]
}
