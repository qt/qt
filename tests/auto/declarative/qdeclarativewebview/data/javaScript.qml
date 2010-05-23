import Qt 4.7
import org.webkit 1.0

WebView {
    url: "javaScript.html"
    javaScriptWindowObjects: [
        QtObject {
            property string qmlprop: "qmlvalue"
            WebView.windowObjectName: "myjsname"
        }
    ]
}
