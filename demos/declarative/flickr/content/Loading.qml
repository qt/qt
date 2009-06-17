Image {
    id: Loading; source: "pics/loading.png"; transformOrigin: "Center"
    rotation: NumericAnimation {
        id: "RotationAnimation"; from: 0; to: 360; running: Loading.visible == true; repeat: true; duration: 900
    }
}
