import Qt 4.7

// Here, we implement "Scale to Fit" behaviour, using the
// fillMode property.
//
Rectangle {
    // default size: whole image, unscaled
    width: face.width
    height: face.height
    color: "gray"
    clip: true

    Image {
        id: face
        smooth: true
        source: "pics/face.png"
        fillMode: Image.PreserveAspectFit
        anchors.fill: parent
    }
}
