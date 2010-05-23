import Qt 4.7
import org.webkit 1.0

WebView {
    width: 200
    height: 250
    url: "zoomTextOnly.html"
    settings.zoomTextOnly: true
    SequentialAnimation on zoomFactor {
        loops: Animation.Infinite
        NumberAnimation { from: 2; to: 0.25; duration: 1000 }
        NumberAnimation { from: 0.25; to: 2; duration: 1000 }
    }
}
