import QtQuick 1.0
import QtWebKit 1.0

Flickable {
    id: flickable
    width: 320
    height:  200
    contentWidth: Math.max(flickable.width,webView.width)
    contentHeight: Math.max(flickable.height,webView.height)
    pressDelay: 100

    WebView {
        id: webView
        transformOrigin: Item.TopLeft
        smooth: false // We don't want smooth scaling, since we only scale during (fast) transitions
        url:  "test.html"
        preferredWidth: flickable.width
        preferredHeight: flickable.height
        contentsScale: 1
        onContentsSizeChanged: {
            // zoom out
            contentsScale = Math.min(1,flickable.width / contentsSize.width)
        }
    }

    Rectangle {
        id: button
        width: 50; height: 50; color: "red"
        MouseArea {
            anchors.fill: parent
            onPressed: button.color = "blue"
            onReleased: button.color = "green"
        }
    }
}
