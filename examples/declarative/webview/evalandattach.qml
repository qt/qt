import Qt 4.6

Item {
    height: 640
    width: 360
    Text {
        id: teksti
        text: webView.statusText1
        anchors.top: parent.top
        height: 30
        anchors.left: parent.left
        width: parent.width/2
    }

    Text {
        id: teksti2
        text: webView.statusText2
        anchors.top: parent.top
        height: 30
        anchors.left: teksti.right
        anchors.right: parent.right
    }

    MouseRegion {
        anchors.fill: teksti
        onClicked: { webView.evaluateJavaScript ("do_it()") }
    }

    WebView {
        id: webView
        property alias statusText1: txt.text
        property alias statusText2: txt2.text
        anchors.top: teksti.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        focus: true
        settings.pluginsEnabled: true
        javaScriptWindowObjects: [
        QtObject {
            id: txt
            WebView.windowObjectName: "statusText1"
            property string text: "Click me!"
        },
        QtObject {
            id: txt2
            WebView.windowObjectName: "statusText2"
            property string text: ""
        }
        ]
        url: "evalandattach.html"
    }

}
