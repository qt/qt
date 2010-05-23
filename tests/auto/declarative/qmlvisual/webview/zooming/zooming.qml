import Qt 4.7
import org.webkit 1.0

// Note that zooming is better done using zoomFactor and careful
// control of rendering to avoid excessive re-rendering during
// zoom animations. This test is written for simplicity.
WebView {
    width: 200
    height: 250
    Behavior on x { NumberAnimation { } }
    Behavior on y { NumberAnimation { } }
    Behavior on scale { NumberAnimation { } }
    url: "zooming.html"
    preferredWidth: width
    preferredHeight: height
    onDoubleClick: {console.log(clickX,clickY);heuristicZoom(clickX,clickY,2)}
    onZoomTo: {console.log(zoom);scale=zoom;x=width/2-centerX;y=height/2-centerY}
}
