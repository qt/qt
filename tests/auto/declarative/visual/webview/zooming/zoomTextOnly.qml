import Qt 4.6

WebView {
    width: 200
    height: 250
    url: "zoomTextOnly.html"
    settings.zoomTextOnly: true
    zoomFactor:
        SequentialAnimation {
            running: true
            repeat: true
            NumberAnimation { from: 2; to: 0.25; duration: 1000 }
            NumberAnimation { from: 0.25; to: 2; duration: 1000 }
        }
}
