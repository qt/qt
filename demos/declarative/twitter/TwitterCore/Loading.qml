import Qt 4.6

Image {
    id: loading; source: "images/loading.png"
    NumberAnimation on rotation {
        from: 0; to: 360; running: loading.visible == true; loops: Qt.Infinite; duration: 900
    }
}
