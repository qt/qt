import Qt 4.7

Item {
    id: container

    property alias image: bg.source
    property alias url: urlText.text

    signal urlEntered(string url)

    width: parent.height; height: parent.height

    BorderImage {
        id: bg; rotation: 180
        x: 8; width: parent.width - 16; height: 30;
        anchors.verticalCenter: parent.verticalCenter
        border { left: 10; top: 10; right: 10; bottom: 10 }
    }

    Rectangle {
        anchors.bottom: bg.bottom
        x: 18; height: 4; color: "#63b1ed"
        width: (bg.width - 20) * webView.progress
        opacity: webView.progress == 1.0 ? 0.0 : 1.0
    }

    TextInput {
        id: urlText
        horizontalAlignment: TextEdit.AlignLeft
        font.pixelSize: 14; focusOnPress: true
        Keys.onEscapePressed: {
            urlText.text = webView.url
            webView.focus = true
        }
        Keys.onReturnPressed: {
            container.urlEntered(urlText.text)
            webView.focus = true
        }
        anchors {
            left: parent.left; right: parent.right; leftMargin: 18; rightMargin: 18
            verticalCenter: parent.verticalCenter
        }
    }
}
