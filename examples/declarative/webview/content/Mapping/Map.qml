import Qt 4.6

Item {
    id: page
    property real latitude: -34.397
    property real longitude: 150.644
    property string address: ""
    WebView {
        id: map
        anchors.fill: parent
        url: "map.html"
        javaScriptWindowObjects: QtObject {
            WebView.windowObjectName: "qml"
            property real lat: page.latitude
            property real lng: page.longitude
            property string address: page.address
            onAddressChanged: {map.evaluateJavaScript("goToAddress()")}
        }
    }
}
