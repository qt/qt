import Qt 4.7

Image {
    property alias editUrl: urlInput.url

    source: "pics/titlebar-bg.png"; fillMode: Image.TileHorizontally

    x: webView.contentX < 0 ? -webView.contentX : webView.contentX > webView.contentWidth-webView.width
       ? -webView.contentX+webView.contentWidth-webView.width : 0

    y: {
        if (webView.progress < 1.0)
            return 0;
        else {
            webView.contentY < 0 ? -webView.contentY : webView.contentY > height ? -height : -webView.contentY
        }
    }

    Column {
        width: parent.width

        Item {
            width: parent.width; height: 20
            Text {
                anchors.centerIn: parent
                text: webView.title; font.pixelSize: 14; font.bold: true
                color: "white"; styleColor: "black"; style: Text.Sunken
            }
        }

        Item {
            width: parent.width; height: 40

            Button {
                id: backButton
                action: webView.back; image: "pics/go-previous-view.png"
                anchors { left: parent.left; bottom: parent.bottom }
            }

            Button {
                id: nextButton
                anchors.left: backButton.right
                action: webView.forward; image: "pics/go-next-view.png"
            }

            UrlInput {
                id: urlInput
                anchors { left: nextButton.right; right: reloadButton.left }
                image: "pics/display.png"
                onUrlEntered: {
                    webBrowser.urlString = url
                    webBrowser.focus = true
                }
            }

            Button {
                id: reloadButton
                anchors { right: parent.right; rightMargin: 4 }
                action: webView.reload; image: "pics/view-refresh.png"; visible: webView.progress == 1.0
            }

            Button {
                id: stopButton
                anchors { right: parent.right; rightMargin: 4 }
                action: webView.stop; image: "pics/edit-delete.png"; visible: webView.progress < 1.0
            }
        }
    }
}
