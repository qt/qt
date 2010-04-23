import Qt 4.7

Image {
    id: loading; source: "pics/loading.png"
    NumberAnimation on rotation {
        from: 0; to: 360; running: loading.visible == true; loops: Animation.Infinite; duration: 900
    }
}
