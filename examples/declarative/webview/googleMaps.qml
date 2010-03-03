// This example demonstrates how Web services such as Google Maps can be
// abstracted as QML types. Here we have a "Mapping" module with a "Map"
// type. The Map type has an address property. Setting that property moves
// the map. The underlying implementation uses WebView and the Google Maps
// API, but users from QML don't need to understand the implementation in
// order to create a Map.

import Qt 4.6
import org.webkit 1.0
import "content/Mapping"

Map {
    id: map
    width: 300
    height: 300
    address: "Paris"
    Rectangle {
        color: "white"
        width: input.width + 20
        height: input.height + 4
        radius: 5
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        x: 70
        TextInput {
            id: input
            text: map.address
            anchors.centerIn: parent
            Keys.onReturnPressed: map.address = input.text
        }
    }
}
