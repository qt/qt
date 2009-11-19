import Qt 4.6

WebView {
    url: "javaScript.html"
    javaScriptWindowObjects: [
        QtObject {
            property string qmlprop: "qmlvalue"
            WebView.windowObjectName: "myjsname"
        }
    ]
}
