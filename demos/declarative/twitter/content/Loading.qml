import Qt 4.6

Image {
    id: loading; source: "images/loading.png"; transformOrigin: "Center"
    rotation: NumberAnimation {
        id: "RotationAnimation"; from: 0; to: 360; running: loading.visible == true; repeat: true; duration: 900
    }
}
