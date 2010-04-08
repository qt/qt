import Qt 4.7

Image {
    id: container
    property bool on: false

    source: "images/busy.png"; visible: container.on
    NumberAnimation on rotation { running: container.on; from: 0; to: 360; loops: Animation.Infinite; duration: 1200 }
}
