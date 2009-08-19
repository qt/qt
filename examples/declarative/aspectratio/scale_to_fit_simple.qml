import Qt 4.6

// Here, we implement "Scale to Fit" behaviour, using the
// preserveAspect property.
//
Rectangle {
    // default size: whole image, unscaled
    width: Image.width
    height: Image.height
    color: "gray"
    clip: true

    Image {
        id: Image
        source: "pics/face.png"
        fillMode: "PreserveAspect"
        anchors.fill: parent
    }
}
