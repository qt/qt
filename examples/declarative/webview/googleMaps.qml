// This example demonstrates how Web services such as Google Maps can be
// abstracted as QML types. Here we have a "Mapping" module with a "Map"
// type. The Map type has an address property. Setting that property moves
// the map. The underlying implementation uses WebView and the Google Maps
// API, but users from QML don't need to understand the implementation in
// order to create a Map.

import Qt 4.7
import org.webkit 1.0
import "content/Mapping"

Map {
    id: map
    width: 300
    height: 300
    address: "Paris"

    Rectangle {
        x: 70
        width: input.width + 20
        height: input.height + 4
        anchors.bottom: parent.bottom; anchors.bottomMargin: 5
        radius: 5
        opacity: map.status == "Ready" ? 1 : 0

        TextInput {
            id: input
            text: map.address
            anchors.centerIn: parent
            Keys.onReturnPressed: map.address = input.text
        }
    }

    Text {
        id: loading
        anchors.centerIn: parent
        text: map.status == "Error" ? "Error" : "Loading"
        opacity: map.status == "Ready" ? 0 : 1
        font.pixelSize: 30

        Behavior on opacity { NumberAnimation{} }
    }
}
