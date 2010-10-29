import QtQuick 1.0
import QtWebKit 1.0

// Note that zooming is better done using zoomFactor and careful
// control of rendering to avoid excessive re-rendering during
// zoom animations. This test is written for simplicity.
WebView {
    property string skip: "WebView tests not counting until resources allocated to WebView maintenance"
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
