import Qt 4.7
import org.webkit 1.0

Item {
    id: page
    property real latitude: -34.397
    property real longitude: 150.644
    property string address: ""
    property alias status: js.status
    WebView {
        id: map
        anchors.fill: parent
        url: "map.html"
        javaScriptWindowObjects: QtObject {
            id: js
            WebView.windowObjectName: "qml"
            property real lat: page.latitude
            property real lng: page.longitude
            property string address: page.address
            property string status: "Loading"
            onAddressChanged: { if (map.url != "" && map.progress==1) map.evaluateJavaScript("goToAddress()") }
        }
        pressGrabTime: 0
        onLoadFinished: { evaluateJavaScript("goToAddress()"); }
    }
}
