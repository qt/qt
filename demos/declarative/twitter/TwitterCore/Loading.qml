import Qt 4.6

Image {
    id: loading; source: "images/loading.png"; transformOrigin: "Center"
    NumberAnimation on rotation {
        from: 0; to: 360; running: loading.visible == true; repeat: true; duration: 900
    }
}
