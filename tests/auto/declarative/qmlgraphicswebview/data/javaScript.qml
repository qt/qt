import Qt 4.6

WebView {
    url: "javaScript.html"
    javaScriptWindowObjects: [
        Object {
            property string qmlprop: "qmlvalue"
            WebView.windowObjectName: "myjsname"
        }
    ]
}
