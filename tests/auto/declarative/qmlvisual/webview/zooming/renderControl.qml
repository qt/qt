import Qt 4.7
import org.webkit 1.0

Rectangle {
    width: 200
    height: 250
    clip: true
    WebView {
        id: webview
        width: 400
        url: "renderControl.html"
        SequentialAnimation on x {
                loops: Animation.Infinite
                NumberAnimation { from: 100; to: 0; duration: 200 }
                PropertyAction { target: webview; property: "renderingEnabled"; value: false }
                NumberAnimation { from: 0; to: -100; duration: 200 }
                PropertyAction { target: webview; property: "renderingEnabled"; value: true }
                NumberAnimation { from: -100; to: 100; duration: 400 }
            }
    }
}
