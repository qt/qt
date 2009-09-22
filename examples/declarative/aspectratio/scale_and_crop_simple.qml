import Qt 4.6

// Here, we implement "Scale to Fit" behaviour, using the
// fillMode property.
//
Rectangle {
    // default size: whole image, unscaled
    width: Face.width
    height: Face.height
    color: "gray"
    clip: true

    Image {
        id: Face
        source: "pics/face.png"
        fillMode: "PreserveAspectCrop"
        anchors.fill: parent
    }
}
