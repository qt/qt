import Qt 4.7
import org.webkit 1.0

WebView {
    width: 200 * zoomFactor
    height: 250 * zoomFactor
    scale: 1/zoomFactor
    url: "resolution.html"
    SequentialAnimation on zoomFactor {
        loops: Animation.Infinite
        NumberAnimation { from: 1; to: 0.25; duration: 2000 }
        NumberAnimation { from: 0.25; to: 1; duration: 2000 }
        NumberAnimation { from: 1; to: 5; duration: 2000 }
        NumberAnimation { from: 5; to: 1; duration: 2000 }
    }
}
